SUBDIRS = 3ds dsi switch

all:
	@for dir in $(SUBDIRS); do $(MAKE) -C $$dir; done

clean:
	@for dir in $(SUBDIRS); do $(MAKE) clean -C $$dir; done

3ds:
	@$(MAKE) -C 3ds

dsi:
	@$(MAKE) -C dsi

switch:
	@$(MAKE) -C switch

.PHONY: $(SUBDIRS) clean