CODE_DIR = src_files

.PHONY: project_code

project_code:
	$(MAKE) -C $(CODE_DIR) vault 
clean:
	$(MAKE) -C $(CODE_DIR) clean

