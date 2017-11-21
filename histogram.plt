set output "vmlatency.pdf"
set terminal pdfcairo solid color enhanced
set datafile separator ","
set title "VM-Entry and VM-Exit turnaround time for Intel Microarchitectures, Cycles"
set auto x
set grid y
set yrange [0:]
set style data histogram
set style histogram cluster gap 1
set style fill solid noborder
set boxwidth 0.9
set xtic rotate by -45 scale 0
set border 3
set ytics nomirror
plot 'vmlatency.dat' using 2:xtic(1) notitle lc rgb "#000080"