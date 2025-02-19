
/*---------------------------------------------------------
* Function: findClosestDate()
* Purpose: Finds the closest tide prediction from the SD
* Params: None
* Returns: interpolated Tide prediction (cm) of whatever times its closest to
---------------------------------------------------------*/
float getTideData(){
  char firstChar;
  String line;
  int curr_year, curr_month, curr_day, curr_hour;
  int itr_year, itr_month, itr_day, itr_hour;
  float prev_itr_pred, itr_pred, prev_itr_hour;

  prev_itr_pred = -1;
  prev_itr_hour = -1;

  if (!SD.begin(SD_CS)){
    Serial.println("Failed to Initialize SD card");
    return NULL;
  }

//  File root = SD.open("/");
//  listFiles(root, 0);
  
  File file = SD.open(TIDE_FILE);
  if (!file){
    Serial.println("Failed to Open Tide File");
    return NULL;
  }

  /* Skip over File Header */
  while (file.available()){
    line = file.readStringUntil('\n');
    firstChar = line[0];
    if (isDigit(firstChar)){
      break;  /* At the first line of data */
    }
  }

  /* Get Current Time */
  curr_year = rtc.getYear();  /* In #### format */
  curr_month = rtc.getMonth();
  curr_day = rtc.getDay();
  curr_hour = rtc.getHours();
  
  /* Start reading tide data */
  while (file.available()){
    // line = file.readStringUntil('\n');  /* Get line of data */
    
    Serial.println(line);
    
    if (line.length() > 0){
      line.trim();  /* Removes whitespaces from end and beginning */

      itr_year = line.substring(2,4).toInt();
      itr_month = line.substring(5,7).toInt();
      itr_day = line.substring(8,10).toInt();
      itr_hour = line.substring(15,17).toInt(); 
      itr_pred = line.substring(21, line.indexOf('\t', 21)).toInt();  // Needs to be updated for military times

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

  Serial.print("Prev hour: ");
  Serial.println(prev_itr_hour);

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

  Serial.print("Hour diff: ");
  Serial.println(hour_diff);
  Serial.print("Scaled Curr Hour: ");
  Serial.println(scaled_curr_hour);

  Serial.print("Itr pred: ");
  Serial.println(itr_pred);
  Serial.print("Prev Itr pred: ");
  Serial.println(prev_itr_pred);

  proportion = scaled_curr_hour / hour_diff;
  pred_diff = itr_pred - prev_itr_pred;
  interpolated_pred = pred_diff * proportion;
  
  Serial.print("Prop: ");
  Serial.println(proportion, 5);
  
  Serial.print("Pred Diff: ");
  Serial.println(pred_diff);

  Serial.print("Interpolated Pred: ");
  Serial.println(interpolated_pred);
  
  file.close();
  return interpolated_pred;
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
