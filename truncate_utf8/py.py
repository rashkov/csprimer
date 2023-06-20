import sys
def longest_common_substring(s1, s2):
    m = [[0] * (1 + len(s2)) for _ in range(1 + len(s1))]
    longest, x_longest = 0, 0
    for x in range(1, 1 + len(s1)):
        for y in range(1, 1 + len(s2)):
            if s1[x - 1] == s2[y - 1]:
                m[x][y] = m[x - 1][y - 1] + 1
                if m[x][y] > longest:
                    longest = m[x][y]
                    x_longest = x
            else:
                m[x][y] = 0
    return s1[x_longest - longest: x_longest]
with open(sys.argv[1], 'r') as f1, open(sys.argv[2], 'r') as f2:
    lines1 = f1.readlines()
    lines2 = f2.readlines()
for line1 in lines1:
    for line2 in lines2:
        lcs = longest_common_substring(line1.strip(), line2.strip())
        if len(lcs) >= 2:
            print(f"File1: \"{line1.strip()}\"\nFile2: \"{line2.strip()}\"\nCommon: \"{lcs}\"")

