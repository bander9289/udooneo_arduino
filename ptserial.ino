#define dbg(...) \
	do { \
		Serial.print("+BADBG: "); \
		_vprint(Serial, __VA_ARGS__); \
	} while (0);

/* recursion's final loop doesn't have any arguments, so end it */
size_t _vprint(HardwareSerial &s)
{
	return s.println("");
}

/* anything that can be printed by Arduino's 'Serial.print' */
template <typename ...TailType, typename HeadType>
size_t _vprint(HardwareSerial &s, HeadType &head, TailType&&... tail)
{
	size_t r = 0;
	r+=s.print(head);
	r+=_vprint(s, (tail)...);
	return r;
}

void setup() {
	Serial.begin(4000000);
	Serial.println("+BASTART");

	Serial0.begin(460800);
}

/* peripheral to host */
void serialEvent0() {
	int len;
	static char buf[33];

	len = Serial0.readBytes(buf, 32);
	if (len > 0) {
		Serial.write(buf, len);
	}
}

static String g_str_val = "unset";

void loop() {
	
	if (Serial.available() > 3) {
		const String &s = Serial.readStringUntil('\n');
		if (s.startsWith("AT+BA")) {
			if (s.startsWith("AT+BASTR=")) {
				g_str_val = s.substring(9);
				Serial.println("\nOK");
			} else if (s.startsWith("AT+BASTR?")) {
				Serial.print("+BASTR: ");
				Serial.println(g_str_val.c_str());
				Serial.println("\nOK");
			} else {
				dbg("unknown cmd: ", s);
				Serial.println("ERROR");
			}
		} else {
			Serial0.write(s.c_str());
		}
	}
}
