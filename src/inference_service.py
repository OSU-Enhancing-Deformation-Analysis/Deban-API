def write_mock_ml_command(file_path="data.txt"):
    recommended_load = 8.5
    confidence = 0.91
    risk_flag = False

    with open(file_path, "w", encoding="utf-8") as f:
        f.write(f"recommended_load={recommended_load}\n")
        f.write(f"confidence={confidence}\n")
        f.write(f"risk_flag={str(risk_flag).lower()}\n")

if __name__ == "__main__":
    write_mock_ml_command()
    print("Mock ML command written to data.txt")