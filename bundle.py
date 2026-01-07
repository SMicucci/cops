import os

SOURCE_DIR = "./src"
OUTPUT_FILE = "cops.h"

def bundle_files():
    if os.path.exists(OUTPUT_FILE):
        try:
            os.remove(OUTPUT_FILE)
            print(f"Cleaned {OUTPUT_FILE}")
        except OSError as e:
            print(f"Error cleaning {OUTPUT_FILE}: {e}")

    print(f"Bundling single libraries from {SOURCE_DIR} ...")

    with open(OUTPUT_FILE, "w", encoding="utf-8") as outfile:
        outfile.write("#ifndef COPS_H\n")
        outfile.write("#define COPS_H\n")
        for root, _, files in os.walk(SOURCE_DIR):
            for filename in sorted(files):
                if not filename.endswith(".h"):
                    continue
                filepath = os.path.join(root, filename)
                print(f" > Processing: {filepath}")
                try:
                    with open(filepath, "r", encoding="utf-8") as infile:
                        lines = infile.readlines()
                        outfile.write(f"/* start import ./src/{filename} */\n")
                        outfile.writelines(lines[1:])
                        outfile.write(f"/* end import ./src/{filename} */\n")
                except Exception as e:
                    print(f"Error reading {filepath}: {e}")
        outfile.write("#endif /* #ifndef COPS_H guard */\n")
        print("")

if __name__ == "__main__":
    bundle_files()
