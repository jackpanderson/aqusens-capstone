import json
import sys

# Define expected JSON structure with types
EXPECTED_SCHEMA = {
    "motor": {
        "reel_radius_cm": float,
        "gear_ratio": float,
        "pulse_per_rev": int
    },
    "position": {
        "narrow_tube_cm": float,
        "tube_cm": float,
        "water_level_cm": float,
        "min_ramp_dist_cm": float,
        "drop_speed_cm_sec": float,
        "raise_speed_cm_sec": float,
        "drop_speeds": list,
        "raise_speeds": list
    },
    "flush": {
        "flush_time": {
            "lift_tube_time_s": float,
            "dump_water_time_s": int,
            "rope_drop_time_s": float,
            "rinse_rope_time_s": float,
            "rinse_tube_time_s": int
        },
        "aqusens_time": {
            "air_gap_time_s": int,
            "water_rinse_time_s": int,
            "last_air_gap_time_s": int
        }
    },
    "sd": {
        "tide_data_name": str,
        "pier_dist_cm": float
    },
    "times": {
        "sample_interval": {"day": int, "hour": int, "min": int, "sec": int},
        "soak_time": {"day": int, "hour": int, "min": int, "sec": int},
        "dry_time": {"day": int, "hour": int, "min": int, "sec": int}
    }
}

# Function to validate JSON structure and types
def validate_json(data, schema, path="root"):
    if not isinstance(data, dict):
        print(f"ERROR: Expected {path} to be a dictionary")
        return False
    
    for key, expected_type in schema.items():
        if key not in data:
            print(f"ERROR: Missing key '{key}' in {path}")
            return False
        
        value = data[key]

        # Handle nested dictionary validation
        if isinstance(expected_type, dict):
            if not validate_json(value, expected_type, path=f"{path}.{key}"):
                return False
        # Handle list validation
        elif expected_type == list:
            if not isinstance(value, list) or not all(isinstance(i, (int, float)) for i in value):
                print(f"ERROR: '{key}' in {path} should be a list of numbers")
                return False
        # Handle standard types
        elif not isinstance(value, expected_type):
            print(f"ERROR: '{key}' in {path} should be {expected_type.__name__}, got {type(value).__name__}")
            return False

    return True

# Function to check value ranges
def validate_ranges(data):
    errors = []

    if data["motor"]["reel_radius_cm"] <= 0:
        errors.append("Motor reel radius must be greater than 0")

    if data["motor"]["pulse_per_rev"] <= 0:
        errors.append("Motor pulse per revolution must be greater than 0")

    if data["position"]["drop_speed_cm_sec"] <= 0:
        errors.append("Drop speed must be greater than 0")

    if data["position"]["raise_speed_cm_sec"] <= 0:
        errors.append("Raise speed must be greater than 0")

    if data["flush"]["flush_time"]["lift_tube_time_s"] < 0:
        errors.append("Lift tube time cannot be negative")

    if len(errors) > 0:
        for error in errors:
            print(f"ERROR: {error}")
        return False
    return True

# Load and validate JSON
def main(json_filename):
    try:
        with open(json_filename, 'r') as file:
            data = json.load(file)
    except json.JSONDecodeError as e:
        print(f"ERROR: Invalid JSON format - {e}")
        sys.exit(1)
    except FileNotFoundError:
        print("ERROR: JSON file not found")
        sys.exit(1)

    # Validate structure
    if not validate_json(data, EXPECTED_SCHEMA):
        sys.exit(1)

    # Validate value ranges
    if not validate_ranges(data):
        sys.exit(1)

    print("JSON configuration is valid!")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python validate_config.py <config.json>")
        sys.exit(1)
    
    main(sys.argv[1])

