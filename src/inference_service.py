# TODO: Get actual data from the ML model, parse is so othat it can
# appropriately fit in the data array.
# TODO: We will need to manually git clone the entire ml-model repo, create a
# small inference ML that can
# TODO: use the .pth file to generate required data that can then be passed
# onto data.txt file.
# TODO: Expected time to take: ~3 hours.

data = [0.82, 32.5, 0.4]

# Write to file
with open("data.txt", "w") as f:
    f.write(" ".join(map(str, data)))
