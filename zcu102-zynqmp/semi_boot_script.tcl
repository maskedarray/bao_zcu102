connect -url TCP:localhost:3121
targets -set -nocase -filter {name =~ "*A53 #0*"}
rst
after 3000
targets -set -nocase -filter {name =~ "*A53 #0*"}

after 500
con
after 2000
targets -set -nocase -filter {name =~ "*A53 #0*"}
# keep image download commented out once it has been downloaded
dow -data my_fat_image.img 0x70000000  
dow -data bao.img 0x200000
after 100