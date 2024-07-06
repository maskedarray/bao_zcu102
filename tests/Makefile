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
	@EXTRA_FLAGS1-"-DCUA_WR" EXTRA_FLAGS2="-DNCUA_WR" bash budget_variation.sh 
budget-variation-wr-rd:
	@echo "WR with RD. Running budget variation with all cores regulated"
	@bash reset_environment.sh
	@EXTRA_FLAGS1="-DCUA_WR" EXTRA_FLAGS2="-DNCUA_RD" bash budget_variation.sh 
single-test:
	@echo "Conducting single test with no modification and default flags. Output will be in ../output.txt"
	@bash reset_environment.sh
	@bash single_test.sh
period-variation-no-period:
	@echo "Period variation test with no period interrupt enabled. Will give result without memguard timer or counter interrupts"
	@bash reset_environment.sh
	@bash period_variation_no_period.sh
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


#cat output.txt | grep -E "Time taken" | cut -d" " -f3
#cat output.txt | grep -E "Cache miss" | cut -d" " -f4
