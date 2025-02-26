#define GMT_TO_PST  (8)
#define JSON_SIZE   (4096)
SDConfig_t sd_cfg = {0};

void setSDCfg(SDConfig_t& cfg) {
  sd_cfg = cfg;
}

void initSD() {
  if (!SD.begin(SD_CS)){
    Serial.println("[SD] Failed to Initialize SD card");
  }
}

/**
 * @brief interpolates tide data from SD card
 * 
 * @return float offset distance for tide in cm
 */
float getTideData(){
  char first_char;
  String line;
  int curr_year, curr_month, curr_day, curr_hour;
  int itr_year, itr_month, itr_day, itr_hour, pred_index;
  float prev_itr_pred, itr_pred, prev_itr_hour;

  String data_print;

  prev_itr_pred = -1;
  prev_itr_hour = -1;
  
  File file = SD.open(sd_cfg.tide_data_name);
  if (!file){
    Serial.println("Failed to Open Tide File");
    return NULL;
  }

  /* Skip over File Header */
  while (file.available()){
    line = file.readStringUntil('\n');
    first_char = line[0];
    if (isDigit(first_char)){
      break;  /* At the first line of data */
    }
  }

  /* Get Current Time */
  curr_year = rtc.getYear();  /* In #### format */
  curr_month = rtc.getMonth();
  curr_day = rtc.getDay();
  curr_hour = rtc.getHours() + GMT_TO_PST; /* Convert GMT tide data to PST */
  
  /* Start reading tide data */
  while (file.available()){
    // line = file.readStringUntil('\n');  /* Get line of data */
    
    // Serial.println(line);
    
    if (line.length() > 0){
      line.trim();  /* Removes whitespaces from end and beginning */

      itr_year = line.substring(2,4).toInt();
      itr_month = line.substring(5,7).toInt();
      itr_day = line.substring(8,10).toInt();
      itr_hour = line.substring(15,17).toInt(); 
      pred_index = line.indexOf('\t', 21) + 2;  /* Offset to move past 2 tabs */
      data_print = line.substring(pred_index);
      itr_pred = line.substring(pred_index, line.indexOf('\t', pred_index)).toInt();  // Needs to be updated for military times

      /* Debugging prints for reading data in from txt file on sd card
      Serial.print("Current year: ");
      Serial.println(curr_year);
      Serial.print("Data year: ");
      Serial.println(itr_year);
      
      Serial.print("Current month: ");
      Serial.println(curr_month);
      Serial.print("Data month: ");
      Serial.println(itr_month);

      Serial.print("Current day: ");
      Serial.println(curr_day);
      Serial.print("Data day: ");
      Serial.println(itr_day);

      Serial.print("Current hour: ");
      Serial.println(curr_hour);
      Serial.print("Data hour: ");
      Serial.println(itr_hour);

      Serial.print("Data PRED: ");
      Serial.println(itr_pred);
      */

      if (curr_year != itr_year){
        Serial.println("Need to update SD Card\n");
        file.close();
        return -1;  /* Need to Update Tide data file */
      }

      if (curr_month == itr_month){
        if (curr_day == itr_day){
          if (curr_hour < itr_hour){
            break;  /* Closest tide data found */
          }
        }
      }

      prev_itr_pred = itr_pred;
      prev_itr_hour = itr_hour;
      line = file.readStringUntil('\n');
    } else {
      file.close();
      return -1; /* Reached end of file */
    }
  }

  /* Interpolate to get an estimate of current tide level */
  float scaled_curr_hour, hour_diff;
  float proportion, pred_diff, interpolated_pred;
  
  if (prev_itr_pred == -1 and prev_itr_hour == -1){
    file.close();
    return itr_pred;  /* First in list is closest match so just return it since there is no prev */
  }

  if (prev_itr_hour > itr_hour){
    /* Need to scale time as interpolation is inbetween two days */
    scaled_curr_hour = (curr_hour + 24) - prev_itr_hour; 
    hour_diff = (itr_hour + 24) - prev_itr_hour;
  } else {
    scaled_curr_hour = curr_hour - prev_itr_hour; 
    hour_diff = itr_hour - prev_itr_hour;
  }

  proportion = scaled_curr_hour / hour_diff;
  pred_diff = itr_pred - prev_itr_pred;
  interpolated_pred = (pred_diff * proportion) + prev_itr_pred;

  /* Debugging prints for calculating interpolation
  Serial.print("Prev hour: ");
  Serial.println(prev_itr_hour);
  
  Serial.print("Hour diff: ");
  Serial.println(hour_diff);
  Serial.print("Scaled Curr Hour: ");
  Serial.println(scaled_curr_hour);

  Serial.print("Itr pred: ");
  Serial.println(itr_pred);
  Serial.print("Prev Itr pred: ");
  Serial.println(prev_itr_pred);
  
  Serial.print("Prop: ");
  Serial.println(proportion, 5);
  
  Serial.print("Pred Diff: ");
  Serial.println(pred_diff);

  Serial.print("Interpolated Pred: ");
  Serial.println(interpolated_pred - prev_itr_pred);
  */
  
  file.close();
  return interpolated_pred;
}


float getDropDistance(){
  float drop_distance_cm;

  Serial.println("T");
  drop_distance_cm = getTideData();

  // get the distance to drop from online or sd card
  // TODO: finish implementing DIVA
  /*
  while (1)
  {
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n'); // Read full line
        drop_distance_cm = data.toFloat();  // Convert to float

        // if drop distance is -1 then get SD card info
        if (drop_distance_cm == -1) {
          drop_distance_cm = getTideData();
        }
        // otherwise convert from meters to cm
        else {
          drop_distance_cm = drop_distance_cm * 100;
        }

        // Flush any remaining characters
        while (Serial.available()) {
            Serial.read();  // Discard extra data
        }
    }
    checkEstop();
  }
  */

  return sd_cfg.pier_dist_cm + drop_distance_cm;
}

void listFiles(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // No more files
      break;
    }
    for (int i = 0; i < numTabs; i++) {
      Serial.print("\t");
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      listFiles(entry, numTabs + 1); // Recursive call for subdirectories
    } else {
      Serial.print("\t");
      Serial.println(entry.size(), DEC); // Print file size
    }
    entry.close();
  }
}


void export_cfg_to_sd() {
    // for debug purposes
    // File root = SD.open("/");
    // listFiles(root, 0);

    File file = SD.open(CONFIG_FILENAME, O_CREAT | O_WRITE | O_TRUNC);
    if (!file) {
        Serial.println("[SD] Error opening file for writing!");
        return;
    }

    GlobalConfig_t& cfg = getGlobalCfg();
    StaticJsonDocument<JSON_SIZE> doc; // TODO: this might be too little

    // time config
    doc["times"]["sample_interval"]["day"] = cfg.times_cfg.sample_interval.day;
    doc["times"]["sample_interval"]["hour"] = cfg.times_cfg.sample_interval.hour;
    doc["times"]["sample_interval"]["min"] = cfg.times_cfg.sample_interval.min;
    doc["times"]["sample_interval"]["sec"] = cfg.times_cfg.sample_interval.sec;

    doc["times"]["soak_time"]["min"] = cfg.times_cfg.soak_time.min;
    doc["times"]["soak_time"]["sec"] = cfg.times_cfg.soak_time.sec;

    doc["times"]["dry_time"]["min"] = cfg.times_cfg.dry_time.min;
    doc["times"]["dry_time"]["sec"] = cfg.times_cfg.dry_time.sec;

    // motor config
    doc["motor"]["reel_radius_cm"] = cfg.motor_cfg.reel_radius_cm;
    doc["motor"]["gear_ratio"] = cfg.motor_cfg.gear_ratio;
    doc["motor"]["pulse_per_rev"] = cfg.motor_cfg.pulse_per_rev;

    // position config
    doc["position"]["narrow_tube_cm"] = cfg.position_cfg.narrow_tube_cm;
    doc["position"]["tube_cm"] = cfg.position_cfg.tube_cm;
    doc["position"]["water_level_cm"] = cfg.position_cfg.water_level_cm;
    doc["position"]["min_ramp_dist_cm"] = cfg.position_cfg.min_ramp_dist_cm;
    doc["position"]["drop_speed_cm_sec"] = cfg.position_cfg.drop_speed_cm_sec;
    doc["position"]["raise_speed_cm_sec"] = cfg.position_cfg.raise_speed_cm_sec;

    JsonArray drop_speeds = doc["position"].createNestedArray("drop_speeds");
    for (float speed : cfg.position_cfg.drop_speeds) drop_speeds.add(speed);

    JsonArray raise_speeds = doc["position"].createNestedArray("raise_speeds");
    for (float speed : cfg.position_cfg.raise_speeds) raise_speeds.add(speed);

    // flush time config
    doc["flush"]["lift_tube_time_s"] = cfg.flush_cfg.flush_time_cfg.lift_tube_time_s;
    doc["flush"]["dump_water_time_s"] = cfg.flush_cfg.flush_time_cfg.dump_water_time_s;
    doc["flush"]["rope_drop_time_s"] = cfg.flush_cfg.flush_time_cfg.rope_drop_time_s;
    doc["flush"]["rinse_rope_time_s"] = cfg.flush_cfg.flush_time_cfg.rinse_rope_time_s;
    doc["flush"]["rinse_tube_time_s"] = cfg.flush_cfg.flush_time_cfg.rinse_tube_time_s;

    // Aqusens timing config
    doc["aqusens"]["air_gap_time_s"] = cfg.flush_cfg.aqusens_time_cfg.air_gap_time_s;
    doc["aqusens"]["water_rinse_time_s"] = cfg.flush_cfg.aqusens_time_cfg.water_rinse_time_s;
    doc["aqusens"]["last_air_gap_time_s"] = cfg.flush_cfg.aqusens_time_cfg.last_air_gap_time_s;

    // SD config
    doc["sd"]["tide_data_name"] = cfg.sd_cfg.tide_data_name;
    doc["sd"]["pier_dist_cm"] = cfg.sd_cfg.pier_dist_cm;

    // write to serial for fun
    // serializeJsonPretty(doc, Serial);

    // Write to file
    if (serializeJsonPretty(doc, file) == 0) {
        Serial.println("[SD] Failed to write JSON to file");
    }

    file.close();
}

bool load_cfg_from_sd(const char* filename) {
    GlobalConfig_t& cfg = getGlobalCfg();

    File file = SD.open(filename, FILE_READ);
    if (!file) {
        Serial.print("Error: Unable to open file ");
        Serial.println(filename);
        return false;
    }

    StaticJsonDocument<JSON_SIZE> doc;
    DeserializationError error = deserializeJson(doc, file);

    if (error) {
        Serial.print("Error: Failed to parse JSON - ");
        Serial.println(error.f_str());
        file.close();
        return false;
    }

    file.close(); 

    // THANKS CHAT for the | operator for this json stuff

    if (doc.containsKey("motor")) {
        cfg.motor_cfg.reel_radius_cm = doc["motor"]["reel_radius_cm"] | 0.0f;
        cfg.motor_cfg.gear_ratio = doc["motor"]["gear_ratio"] | 0.0f;
        cfg.motor_cfg.pulse_per_rev = doc["motor"]["pulse_per_rev"] | 0;
    } else {
        Serial.println("Warning: Missing 'motor' key in JSON.");
    }

    if (doc.containsKey("position")) {
        cfg.position_cfg.narrow_tube_cm = doc["position"]["narrow_tube_cm"] | 0.0f;
        cfg.position_cfg.tube_cm = doc["position"]["tube_cm"] | 0.0f;
        cfg.position_cfg.water_level_cm = doc["position"]["water_level_cm"] | 0.0f;
        cfg.position_cfg.min_ramp_dist_cm = doc["position"]["min_ramp_dist_cm"] | 0.0f;
        cfg.position_cfg.drop_speed_cm_sec = doc["position"]["drop_speed_cm_sec"] | 0.0f;
        cfg.position_cfg.raise_speed_cm_sec = doc["position"]["raise_speed_cm_sec"] | 0.0f;

        JsonArray drop_speeds = doc["position"]["drop_speeds"];
        JsonArray raise_speeds = doc["position"]["raise_speeds"];

        for (size_t i = 0; i < 4 && i < drop_speeds.size(); i++) {
            cfg.position_cfg.drop_speeds[i] = drop_speeds[i] | 0.0f;
        }
        for (size_t i = 0; i < 4 && i < raise_speeds.size(); i++) {
            cfg.position_cfg.raise_speeds[i] = raise_speeds[i] | 0.0f;
        }
    } else {
        Serial.println("Warning: Missing 'position' key in JSON.");
    }

    if (doc.containsKey("flush")) {
        cfg.flush_cfg.flush_time_cfg.lift_tube_time_s = doc["flush"]["lift_tube_time_s"] | 0.0f;
        cfg.flush_cfg.flush_time_cfg.dump_water_time_s = doc["flush"]["dump_water_time_s"] | 0UL;
        cfg.flush_cfg.flush_time_cfg.rope_drop_time_s = doc["flush"]["rope_drop_time_s"] | 0.0f;
        cfg.flush_cfg.flush_time_cfg.rinse_rope_time_s = doc["flush"]["rinse_rope_time_s"] | 0.0f;
        cfg.flush_cfg.flush_time_cfg.rinse_tube_time_s = doc["flush"]["rinse_tube_time_s"] | 0UL;
    } else {
        Serial.println("Warning: Missing 'flush' key in JSON.");
    }

    if (doc.containsKey("aqusens")) {
        cfg.flush_cfg.aqusens_time_cfg.air_gap_time_s = doc["aqusens"]["air_gap_time_s"] | 0UL;
        cfg.flush_cfg.aqusens_time_cfg.water_rinse_time_s = doc["aqusens"]["water_rinse_time_s"] | 0UL;
        cfg.flush_cfg.aqusens_time_cfg.last_air_gap_time_s = doc["aqusens"]["last_air_gap_time_s"] | 0UL;
    } else {
        Serial.println("Warning: Missing 'aqusens' key in JSON.");
    }

    // TODO: fix this for loading in JSON
    // if (doc.containsKey("sd")) {
    //     strlcpy(cfg.sd_cfg.tide_data_name, doc["sd"]["tide_data_name"] | "", sizeof(cfg.sd_cfg.tide_data_name));
    //     cfg.sd_cfg.pier_dist_cm = doc["sd"]["pier_dist_cm"] | 0.0f;
    // } else {
    //     Serial.println("Warning: Missing 'sd' key in JSON.");
    // }

    if (doc.containsKey("times")) {
        cfg.times_cfg.sample_interval.day = doc["times"]["sample_interval"]["day"] | 0;
        cfg.times_cfg.sample_interval.hour = doc["times"]["sample_interval"]["hour"] | 0;
        cfg.times_cfg.sample_interval.min = doc["times"]["sample_interval"]["min"] | 0;
        cfg.times_cfg.sample_interval.sec = doc["times"]["sample_interval"]["sec"] | 0;

        cfg.times_cfg.soak_time.min = doc["times"]["soak_time"]["min"] | 0;
        cfg.times_cfg.soak_time.sec = doc["times"]["soak_time"]["sec"] | 0;

        cfg.times_cfg.dry_time.min = doc["times"]["dry_time"]["min"] | 0;
        cfg.times_cfg.dry_time.sec = doc["times"]["dry_time"]["sec"] | 0;
    } else {
        Serial.println("Warning: Missing 'times' key in JSON.");
    }

    Serial.println("[SD] Config successfully loaded from SD!");
    return true;
}


