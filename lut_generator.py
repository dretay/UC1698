# UC1698 Data_processing lookup table generator
def expand_uc1698(temp):
    # Extract bits and shift exactly like original function
    temp1 = temp & 0x80
    temp2 = (temp & 0x40) >> 3
    temp3 = (temp & 0x20) << 2
    temp4 = (temp & 0x10) >> 1
    temp5 = (temp & 0x08) << 4
    temp6 = (temp & 0x04) << 1
    temp7 = (temp & 0x02) << 6
    temp8 = (temp & 0x01) << 3

    # Expand each bit to 4-bit grayscale
    h11 = temp1 | temp1 >> 1 | temp1 >> 2 | temp1 >> 3
    h12 = temp2 | temp2 >> 1 | temp2 >> 2 | temp2 >> 3
    h13 = temp3 | temp3 >> 1 | temp3 >> 2 | temp3 >> 3
    h14 = temp4 | temp4 >> 1 | temp4 >> 2 | temp4 >> 3
    h15 = temp5 | temp5 >> 1 | temp5 >> 2 | temp5 >> 3
    h16 = temp6 | temp6 >> 1 | temp6 >> 2 | temp6 >> 3
    h17 = temp7 | temp7 >> 1 | temp7 >> 2 | temp7 >> 3
    h18 = temp8 | temp8 >> 1 | temp8 >> 2 | temp8 >> 3

    # Combine into 4 output bytes
    d1 = h11 | h12
    d2 = h13 | h14
    d3 = h15 | h16
    d4 = h17 | h18

    return (d1, d2, d3, d4)

def generate_uc1698_table():
    table = [expand_uc1698(i) for i in range(256)]
    return table

def print_c_array(table):
    print("static const u8 uc1698_lookup[256][4] = {")
    for entry in table:
        print("    {" + ", ".join(f"0x{b:02X}" for b in entry) + "},")
    print("};")

if __name__ == "__main__":
    table = generate_uc1698_table()
    print_c_array(table)
