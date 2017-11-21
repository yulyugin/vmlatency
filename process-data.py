#!/usr/bin/python

import os, sys, re

results_folder="results"

def name2uarch(brand_string):
    brand_string = brand_string.lstrip() # Remove leading spaces
    slised = brand_string.split()
    if slised[0] == "Intel(R)":
        if slised[1] == "Core(TM)2":
            if slised[2] == "CPU":
                return "Merom"
            if slised[2] == "Duo":
                return "Penryn"

        elif slised[1] == "Core(TM)":
            if slised[2] == "i3" or slised[2] == "i5":
                if slised[3] == "CPU":
                    if slised[4] == "760":
                        return "Nehalem"
                    if slised[4] == "U" or slised[4] == "M":
                        return "Westmere"
            elif re.match("i[3|5|7]-2[0-9]{3}", slised[2]):
                return "Sandy Bridge"
            elif re.match("i[3|5|7]-3[0-9]{3}", slised[2]):
                return "Ivy Bridge"
            elif re.match("i[3|5|7]-4[0-9]{3}", slised[2]):
                return "Haswell"
            elif re.match("i[3|5|7]-6[0-9]{3}", slised[2]):
                return "Skylake"
            elif re.match("i[3|5|7]-7[0-9]{3}", slised[2]):
                return "Kaby Lake"

        elif slised[1] == "Xeon(R)":
            if slised[2] == "CPU":
                if slised[3].startswith("X56"):
                    return "Westmere"
                if re.match("E[3|5]-[0-9]{4}", slised[3]):
                    if slised[4] == "0":
                        return "Sandy Bridge"
                    if slised[4] == "v2":
                        return "Ivy Bridge"
                    if slised[4] == "v3":
                        return "Haswell"
                    if slised[4] == "v4":
                        return "Broadwell"
                    if slised[4] == "v5":
                        return "Skylake"

        elif slised[1] == "Celeron(R)":
            if slised[2] == "CPU":
                if slised[3] == "N3150":
                    return "Airmont"
                if slised[3] == "G3900":
                    return "Skylake"

    raise Exception("Unknown architecture %s" % brand_string)

def main():
    datafiles = [f for f in os.listdir(results_folder)
                         if os.path.isfile(os.path.join(results_folder, f)) and
                            f.startswith("Intel(R)") and f.endswith(".txt")]

    cpus = {}
    for datafile in datafiles:
        f = open(os.path.join(results_folder, datafile), 'r')
        cpu = f.readline()[:-1]
        total = 0.
        count = 0
        lmin = sys.maxint
        for l in f.readlines():
            if l.startswith("IA32_"): # Skip MSRs
                continue
            latency = int(l.split(" - ")[1])
            lmin = latency if latency < lmin else lmin
            total += latency
            count += 1
        avg = total / count

        # Skip unreliable results
        if 100. * lmin / avg < 80:
            continue

        
        uarch = name2uarch(cpu)
        if uarch in cpus:
            if lmin < cpus[uarch][1]:
                cpus[uarch] = (avg, lmin)
        else:
            cpus[uarch] = (avg, lmin)

    # Launch dates according to ark.intel.com
    architectures = [("Merom", "Q3'06"), ("Penryn", "Q1'08"),
                     ("Nehalem", "Q1'09"), ("Westmere", "Q1'10"),
                     ("Sandy Bridge", "Q1'11"), ("Ivy Bridge", "Q2'12"),
                     ("Haswell", "Q2'13"), ("Broadwell", "Q3'14"),
                     ("Airmont", "Q1'15"), ("Skylake", "Q3'15"),
                     ("Kaby Lake", "Q3'16")]

    for uarch in cpus.iterkeys():
        if uarch not in [a for a,y in architectures]:
            assert False, "Unknown architecture %s" % uarch

    for arch, date in architectures:
        print "%-13s %d" % (arch + ",", cpus[arch][0])
    return 0

if __name__ == "__main__":
    sys.exit(main())
