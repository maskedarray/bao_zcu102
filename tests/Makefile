budget-variation:
	@echo "RD with RD Running with 4 interfering cores with all cores regulated"
	@bash budget_variation.sh
budget-variation-values:
	@echo "Printing values for budget variation"
	@bash budget_variation_values.sh
budget-variation-single-core:
	@echo "RD Running with single core no interference"
	@EXTRA_FLAGS2=" " bash budget_variation.sh 
budget-variation-rd-wr:
	@echo"RD with WR. Running budget variation with all cores regulated"
	@EXTRA_FLAGS2="-DNCUA_WR" bash budget_variation.sh 
budget-variation-wr-wr:
	@echo"WR with WR. Running budget variation with all cores regulated"
	@EXTRA_FLAGS1-"-DCUA_WR" EXTRA_FLAGS2="-DNCUA_WR" bash budget_variation.sh 
budget-variation-wr-rd:
	@echo"WR with RD. Running budget variation with all cores regulated"
	@EXTRA_FLAGS1="-DCUA_WR" EXTRA_FLAGS2="-DNCUA_RD" bash budget_variation.sh 
