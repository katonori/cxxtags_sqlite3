check:
ifeq (${LLVM_HOME},)
	@echo "ERROR: set LLVM_HOME"
	@exit 1
endif

.cpp.db:
	$(CXXTAGS) $(CXXTAGS_INCLUDES) $< -o $*.db
