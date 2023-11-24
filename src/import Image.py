from PIL import Image


def translate(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)

image = Image.open("D:/DOCUMENTS/Raphael/AVR pc/finch.png")
pixels = image.load()

out_file = open("finch2.txt", "wb")
for y in range(32):
  for x in range(32):
    r = pixels[x, y][0] 
    rRanged = int(translate(r, 0, 255, 0, 31))
    g = pixels[x, y][1]
    gRanged = int(translate(g, 0, 255, 0, 63))
    b = pixels[x, y][2]
    bRanged = int(translate(b, 0, 255, 0, 31))
    #Pixel format : RRRRGGGGGGBBBBB
    RGB = (rRanged << 11) | (gRanged << 5) | bRanged
    out_file.write(bytes(hex(RGB), 'utf-8'))
    out_file.write(bytes(",", 'utf-8'))
