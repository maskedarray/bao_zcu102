connect -url TCP:localhost:3121
targets -set -nocase -filter {name =~ "*A53 #0*"}
rst
after 1000
targets -set -nocase -filter {name =~ "*A53 #0*"}
con
after 2000
targets -set -nocase -filter {name =~ "*A53 #0*"}
dow -data bao.img 0x200000
after 100