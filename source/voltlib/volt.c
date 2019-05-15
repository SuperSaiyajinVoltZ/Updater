/*
*   This file is part of Universal-Manager
*   Copyright (C) 2019 VoltZ, Epicpkmn11, Flame, RocketRobz
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/
/**
 * Volt Lib for Universal-Updater. [UU]
 * @file volt.c
 * @author VoltZ
 * @date 15 April 2019
 * @brief volt implementation
 */

#include "loadbmp.h"
#include "volt.h"

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection;
static C3D_Mtx projectionTopLeft;
static C3D_Mtx projectionTopRight;
static C3D_Mtx projectionBot;
static C3D_Tex* glyphSheets;
static textVertex_s* textVtxArray;
static int textVtxArrayPos;
static C3D_RenderTarget* topLeft;
static C3D_RenderTarget* topRight;
static C3D_RenderTarget* bot;

static struct {
	GPU_TEXTURE_FILTER_PARAM magFilter;
	GPU_TEXTURE_FILTER_PARAM minFilter;
} textureFilters;

static struct {
	size_t id;
	int x;
	int y;
	int xbegin;
	int ybegin;
	int width;
	int height;
	u32 color;
	flipType fliptype;
	float scaleX;
	float scaleY;
	float angle;
	float depth;
	bool initialized;
} textureData;

static struct {
	C3D_Tex tex;
	u32 width;
	u32 height;
	bool allocated;
} textures[MAX_TEXTURES];

static void volt_add_text_vertex(float vx, float vy, float vz, float tx, float ty);
static bool volt_add_quad(int x, int y, int height, int width, float left, float right, float top, float bottom, float depth);
static u32 volt_get_next_pow2(u32 n);
static void volt_get_text_size_internal(float* width, float* height, float scaleX, float scaleY, int wrapX, const char* text);
static void volt_set_text_color(u32 color);

static void volt_add_text_vertex(float vx, float vy, float vz, float tx, float ty)
{
	textVertex_s* vtx = &textVtxArray[textVtxArrayPos++];
	vtx->position[0] = vx;
	vtx->position[1] = vy;
	vtx->position[2] = vz;
	vtx->texcoord[0] = tx;
	vtx->texcoord[1] = ty;
}

void volt_begin_draw(gfxScreen_t target, gfx3dSide_t side)
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	textVtxArrayPos = 0;
	volt_draw_on(target, side);
}

void volt_draw_on(gfxScreen_t target, gfx3dSide_t side)
{
	if (target == GFX_TOP) {
		C3D_FrameDrawOn(side == GFX_LEFT ? topLeft : topRight);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, side == GFX_LEFT ? &projectionTopLeft : &projectionTopRight);
	} else {
		C3D_FrameDrawOn(bot);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projectionBot);
	}
}

void volt_draw_rectangle(int x, int y, int width, int height, u32 color)
{
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_CONSTANT, GPU_CONSTANT, 0);
	C3D_TexEnvOpRgb(env, 0, 0, 0);
	C3D_TexEnvOpAlpha(env, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_RGB, GPU_INTERPOLATE);
	C3D_TexEnvColor(env, color);
	
	if (volt_add_quad(x, y, height, width, 0, 0, 0, 0, DEFAULT_DEPTH))
	{
		C3D_DrawArrays(GPU_TRIANGLE_STRIP, textVtxArrayPos - 4, 4);
	}
}

void volt_draw_text(float x, float y, float scaleX, float scaleY, u32 color, const char* text)
{
	volt_draw_text_wrap(x, y, scaleX, scaleY, color, -1, text);
}

void volt_draw_text_center(gfxScreen_t target, float y, float scaleX, float scaleY, u32 color, const char* text)
{
	float width = volt_get_text_width(text, scaleX, scaleY);
	float x = ((target == GFX_TOP ? TOP_WIDTH : BOTTOM_WIDTH) - width) / 2;
	volt_draw_text(x, y, scaleX, scaleY, color, text);
}

void volt_draw_textf(float x, float y, float scaleX, float scaleY, u32 color, const char* text, ...) 
{
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	volt_draw_text(x, y, scaleX, scaleY, color, buffer);
	va_end(args);
}

void volt_draw_text_wrap(float x, float y, float scaleX, float scaleY, u32 color, float wrapX, const char* text)
{
	if (text == NULL)
		return;
	
	volt_set_text_color(color);

	ssize_t  units;
	uint32_t code;
	const uint8_t* p = (const uint8_t*)text;
	float firstX = x;
	int lastSheet = -1;
	
	do
	{
		if (!*p) break;
		units = decode_utf8(&code, p);
		if (units == -1)
			break;
		p += units;
		if (code == '\n' || (wrapX != -1 && x + scaleX * fontGetCharWidthInfo(fontGlyphIndexFromCodePoint(code))->charWidth >= firstX + wrapX))
		{
			x = firstX;
			y += scaleY*fontGetInfo()->lineFeed;
			p -= code == '\n' ? 0 : 1;
		}
		else if (code > 0)
		{
			int glyphIdx = fontGlyphIndexFromCodePoint(code);
			fontGlyphPos_s data;
			fontCalcGlyphPos(&data, glyphIdx, GLYPH_POS_CALC_VTXCOORD, scaleX, scaleY);

			if (data.sheetIndex != lastSheet)
			{
				lastSheet = data.sheetIndex;
				C3D_TexBind(0, &glyphSheets[lastSheet]);
			}

			if ((textVtxArrayPos+4) >= TEXT_VTX_ARRAY_COUNT)
				break;

			volt_add_text_vertex(x+data.vtxcoord.left,  y+data.vtxcoord.bottom, DEFAULT_DEPTH, data.texcoord.left,  data.texcoord.bottom);
			volt_add_text_vertex(x+data.vtxcoord.right, y+data.vtxcoord.bottom, DEFAULT_DEPTH, data.texcoord.right, data.texcoord.bottom);
			volt_add_text_vertex(x+data.vtxcoord.left,  y+data.vtxcoord.top,    DEFAULT_DEPTH, data.texcoord.left,  data.texcoord.top);
			volt_add_text_vertex(x+data.vtxcoord.right, y+data.vtxcoord.top,    DEFAULT_DEPTH, data.texcoord.right, data.texcoord.top);

			C3D_DrawArrays(GPU_TRIANGLE_STRIP, textVtxArrayPos - 4, 4);

			x += data.xAdvance;
		}
	} while (code > 0);
}

void volt_draw_texture(size_t id, int x, int y)
{
	volt_texture_select(id, x, y);
	volt_texture_draw();
}

void volt_draw_texture_blend(size_t id, int x, int y, u32 color)
{
	volt_texture_select(id, x, y);
	volt_texture_blend(color);
	volt_texture_draw();
}

void volt_draw_texture_flip(size_t id, int x, int y, flipType fliptype)
{
	volt_texture_select(id, x, y);
	volt_texture_flip(fliptype);
	volt_texture_draw();
}

void volt_draw_texture_rotate(size_t id, int x, int y, float angle)
{
	volt_texture_select(id, x, y);
	volt_texture_rotate(angle);
	volt_texture_draw();		
}

void volt_draw_texture_scale(size_t id, int x, int y, float scaleX, float scaleY)
{
	volt_texture_select(id, x, y);
	volt_texture_scale(scaleX, scaleY);
	volt_texture_draw();
}

void volt_draw_texture_part(size_t id, int x, int y, int xbegin, int ybegin, int width, int height)
{
	volt_texture_select_part(id, x, y, xbegin, ybegin, width, height);
	volt_texture_draw();
}

void volt_draw_wtext(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t* text) 
{
	volt_draw_wtext_wrap(x, y, scaleX, scaleY, color, -1, text);
}

void volt_draw_wtext_center(gfxScreen_t target, float y, float scaleX, float scaleY, u32 color, const wchar_t* text)
{
	float width = volt_get_wtext_width(text, scaleX, scaleY);
	float x = ((target == GFX_TOP ? TOP_WIDTH : BOTTOM_WIDTH) - width) / 2;
	volt_draw_wtext(x, y, scaleX, scaleY, color, text);
}

void volt_draw_wtext_wrap(float x, float y, float scaleX, float scaleY, u32 color, float wrapX, const wchar_t* text) 
{
	if (text == NULL)
		return;
	
	u32 size = wcslen(text) * sizeof(wchar_t);
	char buf[size];
	memset(buf, 0, size);
	utf32_to_utf8((uint8_t*)buf, (uint32_t*)text, size);
	buf[size - 1] = '\0';	
	
	volt_draw_text_wrap(x, y, scaleX, scaleY, color, wrapX, buf);
}

void volt_draw_wtextf(float x, float y, float scaleX, float scaleY, u32 color, const wchar_t* text, ...) 
{
	wchar_t buffer[256];
	va_list args;
	va_start(args, text);
	vswprintf(buffer, 256, text, args);
	volt_draw_wtext(x, y, scaleX, scaleY, color, buffer);
	va_end(args);
}

void volt_end_draw(void)
{
	C3D_FrameEnd(0);
}

void volt_exit(void)
{
	for (size_t id = 0; id < MAX_TEXTURES; id++)
	{
		volt_free_texture(id);
	}
	
	linearFree(textVtxArray);
	free(glyphSheets);
	
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
	
	C3D_Fini();
	gfxExit();
}

static bool volt_add_quad(int x, int y, int height, int width, float left, float right, float top, float bottom, float depth)
{
	if ((textVtxArrayPos+4) >= TEXT_VTX_ARRAY_COUNT)
		return false;
	
	volt_add_text_vertex(        x, y + height, depth,  left, bottom);
	volt_add_text_vertex(x + width, y + height, depth, right, bottom);
	volt_add_text_vertex(        x,          y, depth,  left,    top);
	volt_add_text_vertex(x + width,          y, depth, right,    top);
	
	return true;
}

void volt_free_texture(size_t id)
{
	if (id >= MAX_TEXTURES)
		return;
	
	if (!textures[id].allocated)
		return;
	
	C3D_TexDelete(&textures[id].tex);
	textures[id].width = 0;
	textures[id].height = 0;
	textures[id].allocated = false;
}

Result volt_init(void)
{
	Result res = 0;
	
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	
	topLeft = C3D_RenderTargetCreate(SCREEN_HEIGHT, TOP_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetClear(topLeft, C3D_CLEAR_ALL, BACKGROUND_COLOR, 0);
	C3D_RenderTargetSetOutput(topLeft, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	
	topRight = C3D_RenderTargetCreate(SCREEN_HEIGHT, TOP_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetClear(topRight, C3D_CLEAR_ALL, BACKGROUND_COLOR, 0);
	C3D_RenderTargetSetOutput(topRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);
	
	bot = C3D_RenderTargetCreate(SCREEN_HEIGHT, BOTTOM_WIDTH, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetClear(bot, C3D_CLEAR_ALL, BACKGROUND_COLOR, 0);
	C3D_RenderTargetSetOutput(bot, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	
	res = fontEnsureMapped();
	if (R_FAILED(res))
		return res;
	
	volt_set_texture_filter(GPU_NEAREST, GPU_NEAREST);

#ifdef BUILDTOOLS
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_len);
#else
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
#endif

	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);

	uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	
	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3);
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2);

	Mtx_OrthoTilt(&projectionTopLeft, 0, TOP_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, true);
	Mtx_OrthoTilt(&projectionTopRight, 0, TOP_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, true);
	Mtx_OrthoTilt(&projectionBot, 0, BOTTOM_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, true);
	
	C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

	int i;
	TGLP_s* glyphInfo = fontGetGlyphInfo();
	glyphSheets = malloc(sizeof(C3D_Tex)*glyphInfo->nSheets);
	for (i = 0; i < glyphInfo->nSheets; i ++)
	{
		C3D_Tex* tex = &glyphSheets[i];
		tex->data = fontGetGlyphSheetTex(i);
		tex->fmt = glyphInfo->sheetFmt;
		tex->size = glyphInfo->sheetSize;
		tex->width = glyphInfo->sheetWidth;
		tex->height = glyphInfo->sheetHeight;
		tex->param = GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)
			| GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_EDGE) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_EDGE);
		tex->border = 0;
		tex->lodParam = 0;
	}
	
	textVtxArray = (textVertex_s*)linearAlloc(sizeof(textVertex_s)*TEXT_VTX_ARRAY_COUNT);
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, textVtxArray, sizeof(textVertex_s), 2, 0x10);
	
	return 0;
}

static u32 volt_get_next_pow2(u32 v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v >= 64 ? v : 64;
}

float volt_get_text_height(const char* text, float scaleX, float scaleY)
{
	float height;
	volt_get_text_size_internal(NULL, &height, scaleX, scaleY, -1, text);
	return height;
}

float volt_get_text_height_wrap(const char* text, float scaleX, float scaleY, int wrapX)
{
	float height;
	volt_get_text_size_internal(NULL, &height, scaleX, scaleY, wrapX, text);
	return height;
}

void volt_get_text_size(float* width, float* height, float scaleX, float scaleY, const char* text)
{
	volt_get_text_size_internal(width, height, scaleX, scaleY, -1, text);
}

static void volt_get_text_size_internal(float* width, float* height, float scaleX, float scaleY, int wrapX, const char* text)
{
    float maxW = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
    
    ssize_t  units;
    uint32_t code;
    float x = 0;
    float firstX = x;
    const uint8_t* p = (const uint8_t*)text;
    
    do
    {
        if (!*p) break;
        units = decode_utf8(&code, p);
        if (units == -1)
            break;
        p += units;
        if (code == '\n' || (wrapX != -1 && x + scaleX * fontGetCharWidthInfo(fontGlyphIndexFromCodePoint(code))->charWidth >= firstX + wrapX))
        {
            x = firstX;
            h += scaleY*fontGetInfo()->lineFeed;
            p -= code == '\n' ? 0 : 1;
            if (w > maxW)
                maxW = w;
            w = 0.f;
        }
        else if (code > 0)
        {
            float len = (scaleX * fontGetCharWidthInfo(fontGlyphIndexFromCodePoint(code))->charWidth);
            w += len;
            x += len;
        }
    } while (code > 0);
    
    if (width)
    {
        *width = w > maxW ? w : maxW;
    }
    
    if (height)
    {
        h += scaleY*fontGetInfo()->lineFeed;
        *height = h;
    }
}

float volt_get_text_width(const char* text, float scaleX, float scaleY)
{
	float width;
	volt_get_text_size_internal(&width, NULL, scaleX, scaleY, -1, text);
	return width;
}

float volt_get_wtext_height(const wchar_t* text, float scaleX, float scaleY)
{
	u32 size = wcslen(text) * sizeof(wchar_t);
	char buf[size];
	memset(buf, 0, size);
	utf32_to_utf8((uint8_t*)buf, (uint32_t*)text, size);
	buf[size - 1] = '\0';
	
	float height;
	volt_get_text_size_internal(NULL, &height, scaleX, scaleY, -1, buf);
	return height;
}

float volt_get_wtext_width(const wchar_t* text, float scaleX, float scaleY)
{
	u32 size = wcslen(text) * sizeof(wchar_t);
	char buf[size];
	memset(buf, 0, size);
	utf32_to_utf8((uint8_t*)buf, (uint32_t*)text, size);
	buf[size - 1] = '\0';
	
	float width;
	volt_get_text_size_internal(&width, NULL, scaleX, scaleY, -1, buf);
	return width;
}

void volt_load_texture_bmp(size_t id, const char* path)
{
	if (id >= MAX_TEXTURES)
		return;
	
	u8* image = NULL;
	unsigned int width = 0, height = 0;
	loadbmp_decode_file(path, &image, &width, &height);
	
	volt_load_texture_memory(id, image, width, height);
	free(image);
}

void volt_load_texture_memory(size_t id, void* buf, u32 width, u32 height)
{
	u32 w_pow2 = volt_get_next_pow2(width);
	u32 h_pow2 = volt_get_next_pow2(height);
	
	C3D_TexInit(&textures[id].tex, (u16)w_pow2, (u16)h_pow2, GPU_RGBA8);
	C3D_TexSetFilter(&textures[id].tex, textureFilters.magFilter, textureFilters.minFilter);
	
	textures[id].allocated = true;
	textures[id].width = width;
	textures[id].height = height;

	memset(textures[id].tex.data, 0, textures[id].tex.size);
	for (u32 i = 0; i < width; i++) 
	{
		for (u32 j = 0; j < height; j++) 
		{
			u32 dst = ((((j >> 3) * (w_pow2 >> 3) + (i >> 3)) << 6) + ((i & 1) | ((j & 1) << 1) | ((i & 2) << 1) | ((j & 2) << 2) | ((i & 4) << 2) | ((j & 4) << 3))) * 4;
			u32 src = (j * width + i) * 4;

			memcpy(textures[id].tex.data + dst, buf + src, 4);
		}
	}

	C3D_TexFlush(&textures[id].tex);	
}

void volt_load_texture_memory_RGBA5551(size_t id, void* buf, u32 width, u32 height)
{
	u32 w_pow2 = volt_get_next_pow2(width);
	u32 h_pow2 = volt_get_next_pow2(height);
	
	C3D_TexInit(&textures[id].tex, (u16)w_pow2, (u16)h_pow2, GPU_RGBA5551);
	C3D_TexSetFilter(&textures[id].tex, textureFilters.magFilter, textureFilters.minFilter);
	
	textures[id].allocated = true;
	textures[id].width = width;
	textures[id].height = height;

	memset(textures[id].tex.data, 0, textures[id].tex.size);
	for (u32 i = 0; i < width; i++) 
	{
		for (u32 j = 0; j < height; j++) 
		{
			u32 dst = ((((j >> 3) * (w_pow2 >> 3) + (i >> 3)) << 6) + ((i & 1) | ((j & 1) << 1) | ((i & 2) << 1) | ((j & 2) << 2) | ((i & 4) << 2) | ((j & 4) << 3))) * 2;
			u32 src = (j * width + i) * 2;

			memcpy(textures[id].tex.data + dst, buf + src, 2);
		}
	}

	C3D_TexFlush(&textures[id].tex);	
}

void volt_load_texture_png(size_t id, const char* path)
{
	if (id >= MAX_TEXTURES)
		return;
	
	u8* image;
	unsigned width, height;

	lodepng_decode32_file(&image, &width, &height, path);
	for (u32 i = 0; i < width; i++) 
	{
		for (u32 j = 0; j < height; j++) 
		{
			u32 p = (i + j*width) * 4;

			u8 r = *(u8*)(image + p);
			u8 g = *(u8*)(image + p + 1);
			u8 b = *(u8*)(image + p + 2);
			u8 a = *(u8*)(image + p + 3);

			*(image + p) = a;
			*(image + p + 1) = b;
			*(image + p + 2) = g;
			*(image + p + 3) = r;
		}
	}
	
	volt_load_texture_memory(id, image, width, height);
	free(image);
}

void volt_load_texture_png_memory(size_t id, void* buf, size_t buf_size)
{
	if (id >= MAX_TEXTURES)
		return;

	u8* image;
	unsigned width;
	unsigned height;

	lodepng_decode32(&image, &width, &height, buf, buf_size);

	for (u32 i = 0; i < width; i++)
	{
		for (u32 j = 0; j < height; j++)
		{
			u32 p = (i + j*width) * 4;

			u8 r = *(u8*)(image + p);
			u8 g = *(u8*)(image + p + 1);
			u8 b = *(u8*)(image + p + 2);
			u8 a = *(u8*)(image + p + 3);

			*(image + p) = a;
			*(image + p + 1) = b;
			*(image + p + 2) = g;
			*(image + p + 3) = r;
		}
	}

	volt_load_texture_memory(id, image, width, height);
	free(image);
}

void volt_set_3D(int enable)
{
	gfxSet3D(enable);
}

void volt_set_screen_color(gfxScreen_t target, u32 color)
{
	if (target == GFX_TOP)
	{
		C3D_RenderTargetClear(topLeft, C3D_CLEAR_ALL, color, 0);
		C3D_RenderTargetClear(topRight, C3D_CLEAR_ALL, color, 0);
	}
	else
	{
		C3D_RenderTargetClear(bot, C3D_CLEAR_ALL, color, 0);
	}
}

void volt_set_texture_filter(GPU_TEXTURE_FILTER_PARAM magFilter, GPU_TEXTURE_FILTER_PARAM minFilter)
{
	textureFilters.magFilter = magFilter;
	textureFilters.minFilter = minFilter;
}

static void volt_set_text_color(u32 color)
{
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_RGB, GPU_CONSTANT, 0, 0);
	C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0, GPU_CONSTANT, 0);
	C3D_TexEnvOpRgb(env, 0, 0, 0);
	C3D_TexEnvOpAlpha(env, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
	C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
	C3D_TexEnvColor(env, color);
}

void volt_texture_select(size_t id, int x, int y)
{
	if (id >= MAX_TEXTURES)
	{
		textureData.initialized = false;
		return;
	}
	
	textureData.id = id;
	textureData.x = x;
	textureData.y = y;
	textureData.xbegin = 0;
	textureData.ybegin = 0;
	textureData.width = textures[id].width;
	textureData.height = textures[id].height;
	textureData.color = VOLT_NEUTRAL;
	textureData.fliptype = NONE;
	textureData.scaleX = 1;
	textureData.scaleY = 1;
	textureData.angle = 0;
	textureData.depth = DEFAULT_DEPTH;
	textureData.initialized = true;
}

void volt_texture_select_part(size_t id, int x, int y, int xbegin, int ybegin, int width, int height)
{
	if (id >= MAX_TEXTURES)
	{
		textureData.initialized = false;
		return;
	}
	
	textureData.id = id;
	textureData.x = x;
	textureData.y = y;
	textureData.xbegin = xbegin;
	textureData.ybegin = ybegin;
	textureData.width = width;
	textureData.height = height;
	textureData.color = VOLT_NEUTRAL;
	textureData.fliptype = NONE;
	textureData.scaleX = 1;
	textureData.scaleY = 1;
	textureData.angle = 0;
	textureData.depth = DEFAULT_DEPTH;
	textureData.initialized = true;
}

void volt_texture_blend(u32 color)
{
	textureData.color = color;
}

void volt_texture_scale(float scaleX, float scaleY)
{
	textureData.scaleX = scaleX;
	textureData.scaleY = scaleY;
}

void volt_texture_flip(flipType fliptype)
{
	textureData.fliptype = fliptype;
}

void volt_texture_rotate(float angle)
{
	textureData.angle = angle;
}

void volt_texture_depth(float depth)
{
	textureData.depth = depth;
}

void volt_texture_draw(void)
{
	if (!textureData.initialized)
		return;

	if ((textVtxArrayPos+4) >= TEXT_VTX_ARRAY_COUNT)
		return;
	
	size_t id = textureData.id;
	
	float left = (float)textureData.xbegin / (float)textures[id].tex.width;
	float right = (float)(textureData.xbegin + textureData.width) / (float)textures[id].tex.width;
	float top = (float)(textures[id].tex.height - textureData.ybegin) / (float)textures[id].tex.height;
	float bottom = (float)(textures[id].tex.height - textureData.ybegin - textureData.height) / (float)textures[id].tex.height;
	
	// scaling
	textureData.height *= textureData.scaleY;
	textureData.width *= textureData.scaleX;
	
	float vert[4][2] = {
		{                    textureData.x, textureData.height + textureData.y},
		{textureData.width + textureData.x, textureData.height + textureData.y},
		{                    textureData.x,                      textureData.y},
		{textureData.width + textureData.x,                      textureData.y},
	};

	// flipping
	if (textureData.fliptype == BOTH || textureData.fliptype == HORIZONTAL)
	{
		float tmp = left;
		left = right;
		right = tmp;
	}
	
	if (textureData.fliptype == BOTH || textureData.fliptype == VERTICAL)
	{
		float tmp = top;
		top = bottom;
		bottom = tmp;
	}
	
	// rotating
	textureData.angle = fmod(textureData.angle, 360);
	if (textureData.angle != 0)
	{
		const float rad = textureData.angle/(180/M_PI);
		const float c = cosf(rad);
		const float s = sinf(rad);
		
		const float xcenter = textureData.x + textureData.width/2.0f;
		const float ycenter = textureData.y + textureData.height/2.0f;
		
		for (int i = 0; i < 4; i++)
		{
			float oldx = vert[i][0];
			float oldy = vert[i][1];
			
			float newx = c * (oldx - xcenter) - s * (oldy - ycenter) + xcenter;
			float newy = s * (oldx - xcenter) + c * (oldy - ycenter) + ycenter;
			
			vert[i][0] = newx;
			vert[i][1] = newy;
		}
	}

	// blending
	C3D_TexBind(0, &textures[id].tex);
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_CONSTANT, 0);
	C3D_TexEnvOpRgb(env, 0, 0, 0);
	C3D_TexEnvOpAlpha(env, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
	C3D_TexEnvColor(env, textureData.color);

	// rendering
	volt_add_text_vertex(vert[0][0], vert[0][1], textureData.depth, left, bottom);
	volt_add_text_vertex(vert[1][0], vert[1][1], textureData.depth, right, bottom);
	volt_add_text_vertex(vert[2][0], vert[2][1], textureData.depth, left, top);
	volt_add_text_vertex(vert[3][0], vert[3][1], textureData.depth, right, top);

	C3D_DrawArrays(GPU_TRIANGLE_STRIP, textVtxArrayPos - 4, 4);
}