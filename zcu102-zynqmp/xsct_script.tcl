


while {1} {
    set errorCode ""
    set errorMsg  ""

    if {[catch {
        connect -url TCP:localhost:3121
        targets -set -nocase -filter {name =~ "*PSU*"}
        rst
        after 500
        targets -set -nocase -filter {name =~ "*PSU*"}
        rst
        after 500
        targets -set -nocase -filter {name =~ "*PS TAP*"}
        fpga "download.bit"
        targets -set -nocase -filter {name =~ "*PSU*"} 
        mask_write 0xFFCA0038 0x1C0 0x1C0
        targets -set -nocase -filter {name =~ "*MicroBlaze PMU*"}
        dow "pmufw.elf"
        con

        after 1000

        targets -set -nocase -filter {name =~ "*PSU*"}
        mask_write 0xFFCA0038 0x1C0 0x0

        after 1000

        targets -set -nocase -filter {name =~ "*APU*"}
        mwr 0xffff0000 0x14000000
        mask_write 0xFD1A0104 0x501 0x0

        after 1000

        source psu_init.tcl
        psu_init

        after 1000

        targets -set -nocase -filter {name =~ "*A53 #0*"}
        dow "zynqmp_fsbl.elf"
        con
        after 4000; stop
        targets -set -nocase -filter {name =~ "*A53 #0*"}
        dow -data "system.dtb" 0x100000
        targets -set -nocase -filter {name =~ "*A53 #0*"}
        dow  "u-boot.elf"
        targets -set -nocase -filter {name =~ "*A53 #0*"}
        dow  "bl31.elf"
        dow -data  bao.img 0x200000
        

    } errorMsg]} {
        puts "Error: $errorCode - $errorMsg"
        puts "Attempting to re-execute the script..."
        after 1000 ;# Add a delay before the next attempt (adjust as needed)
    } else {
        break ;# No error occurred, exit the loop
    }
}


