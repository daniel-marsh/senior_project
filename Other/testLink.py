import sys

def test(c):
    f = open("test_link.txt", "w")
    f.write(str(c))
    f.close()
    return c

if __name__ == "__main__":
    test(sys.argv[1])