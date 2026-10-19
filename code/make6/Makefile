TARGET = output.txt

$(TARGET):
	echo "Generating file..." > $(TARGET)
	echo "This is a test file." >> $(TARGET)

show: $(TARGET)
	cat $(TARGET)

clean:
	rm -f $(TARGET)