budget-variation:
	@echo "RD with RD Running with 4 interfering cores with all cores regulated"
	@bash reset_environment.sh
	@bash budget_variation.sh
budget-variation-values:
	@echo "Printing values for budget variation"
	@bash reset_environment.sh
	@bash budget_variation_values.sh
budget-variation-single-core:
	@echo "RD Running with single core no interference"
	@bash reset_environment.sh
	@EXTRA_FLAGS2=" " bash budget_variation.sh 
budget-variation-rd-wr:
	@echo "RD with WR. Running budget variation with all cores regulated"
	@bash reset_environment.sh
	@EXTRA_FLAGS2="-DNCUA_WR" bash budget_variation.sh 
budget-variation-wr-wr:
	@echo "WR with WR. Running budget variation with all cores regulated"
	@bash reset_environment.sh
	@EXTRA_FLAGS1="-DCUA_WR" EXTRA_FLAGS2="-DNCUA_WR" bash budget_variation.sh 
budget-variation-wr-rd:
	@echo "WR with RD. Running budget variation with all cores regulated"
	@bash reset_environment.sh
	@EXTRA_FLAGS1="-DCUA_WR" EXTRA_FLAGS2="-DNCUA_RD" bash budget_variation.sh 
single-test:
	@echo "Conducting single test with no modification and default flags. Output will be in ../output.txt"
	@bash reset_environment.sh
	@bash single_test.sh
no-memguard-single-run:
	@echo "Single run without memguard"
	@bash reset_environment.sh
	@bash no_memguard_single_run.sh
period-variation: 
	@echo "Period variation test, starts from Period defined in period_variation.sh and has 20 trials"
	@bash reset_environment.sh
	@bash period_variation.sh

freertos-over-bao:
	@echo "Running Freertos over bao. Without memguard timer or period interrupt"
	@bash reset_environment.sh
	@bash freertos-over-bao.sh

freertos-over-bao-static:
	@echo "Upload image using make upload-fat-image before executing this target"
	@echo "Running Freertos over bao. Without memguard timer or period interrupt"
	@bash reset_environment.sh
	@EXTRA_FLAGS1="-DCUA_RD -DSTATIC_FAT_IMG" EXTRA_FLAGS2="-DNCUA_RD" HYP_FLAGS="-DPERIOD_VARIATION_NO_PERIOD -DSTATIC_FAT_IMG" bash freertos-over-bao.sh

budget-variation-freertos-wr:
	@echo "Benchmark with WR Running with 4 interfering cores with all cores regulated"
	@bash reset_environment.sh
	@EXTRA_FLAGS1="-DCUA_RD" EXTRA_FLAGS2="-DNCUA_WR" HYP_FLAGS="" bash budget_variation_freertos.sh

budget-variation-freertos-wr-static:
	@echo "Benchmark with WR Running with 4 interfering cores with all cores regulated"
	@bash reset_environment.sh
	@EXTRA_FLAGS1="-DCUA_RD -DSTATIC_FAT_IMG" EXTRA_FLAGS2="-DNCUA_WR" HYP_FLAGS="-DSTATIC_FAT_IMG" bash budget_variation_freertos.sh

upload-static-img:
	@echo "Uploading FAT image to 0x70000000"
	@bash upload_img.sh

color-variation-freertos:
	@echo "Running color varitation test. Warning: Lines numbers for config.c are hardcoded. Script depends on budget_variation_freertos.sh"
	@bash config_color_variation.sh

color-variation-synth:
	@echo "Running color varitation test. Warning: Lines numbers for config.c are hardcoded. Script depends on budget_variation_synthetic.sh"
	@bash config_color_variation_synth.sh

#cat output.txt | grep -E "Time taken" | cut -d" " -f3
#cat output.txt | grep -E "Cache miss" | cut -d" " -f4
