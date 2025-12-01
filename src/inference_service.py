data = [10, 20, 30, 40]

# Write to file
with open("data.txt", "w") as f:
    f.write(" ".join(map(str, data)))
