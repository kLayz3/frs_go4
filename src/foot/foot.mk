FOOT_DIR=foot
FOOT_GEN=$(FOOT_DIR)/gen_hh.sh
#DPTC_PATH=$(FOOT_DIR)/dptc
#DPTC_FILE_PREFIX=dptc_unpack
#FRS_EXEO += $(DPTC_PATH)/$(DPTC_FILE_PREFIX).o

foot_hh:
	./$(FOOT_GEN)
#	gcc -c $(DPTC_PATH)/$(DPTC_FILE_PREFIX).c -o $(DPTC_PATH)/$(DPTC_FILE_PREFIX).o
#	ln -sf $(DPTC_PATH)/$(DPTC_FILE_PREFIX).o $(DPTC_FILE_PREFIX).o 
