import random
import string
import uuid
import datetime
import sys

# how many string literals you want
N = 1000

# some "realistic" base strings you’d see in scripts/programs
base_literals = [
    "Hello, world!",
    "error: file not found",
    "ok",
    "true",
    "false",
    "null",
    "NaN",
    "Infinity",
    "admin",
    "user",
    "password123",
    "debug",
    "info",
    "warning",
    "critical",
    "session expired",
    "connection reset",
    "/usr/local/bin",
    "C:\\Program Files\\App",
    "tmp/file.log",
    "config.json",
    "settings.ini",
    "https://example.com",
    "http://localhost:8080",
    "127.0.0.1",
    "0.0.0.0",
    "<html><body>Test</body></html>",
    "{\"status\": \"ok\"}",
    "[1,2,3]",
    "SELECT * FROM users;",
    "INSERT INTO logs VALUES (1, 'ok');",
    "π = 3.14159",
    "Δx = 0.001",
    "😀 😎 👍",
    "token=abc123",
    "uuid=550e8400-e29b-41d4-a716-446655440000",
    "2025-08-24",
    "line one\\nline two",
    "tab\\tseparated"
]

def random_string():
    """Generate a semi-realistic random string"""
    choice = random.randint(1, 6)
    if choice == 1:
        return ''.join(random.choices(string.ascii_letters + string.digits, k=random.randint(5, 15)))
    elif choice == 2:
        return str(uuid.uuid4())
    elif choice == 3:
        return datetime.date.today().isoformat()
    elif choice == 4:
        return f"file_{random.randint(1,9999)}.txt"
    elif choice == 5:
        return f"user{random.randint(1,9999)}@example.com"
    else:
        return random.choice(base_literals)

# generate list
strings = [random_string() for _ in range(N)]

# --- OPTION 1: just print them ---
for s in strings:
    print(s, file=sys.stdout, flush=True)

# --- OPTION 2: output as C string array ---
#with open("string_literla.c", "w", encoding="utf-8") as f:
# f.write("const char* test_strings[] = {\n")
# for s in strings:
#     escaped = s.replace("\\", "\\\\").replace("\"", "\\\"")
#     f.write(f"    \"{escaped}\",\n")
# f.write("};\n")
