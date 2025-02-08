/**
 * @brief Finds the closest tide prediction from the SD
 * 
 * @param nextDropTime String with next drop time in correct format
 * @return double Tide prediction of whatever time is closest
 */
double findClosestDate(String nextDropTime) {
  String closestDate = "";
  char currentChar, previousChar;
  String year, month, day, time, pred, compDate, line, prevPred, prevCompDate;

  if (!SD.begin(SD_CS)) {
    Serial.println("Initialization failed!");
    return NULL;
  }

  File file = SD.open(TIDE_FILE);
  if (!file) {
    Serial.println("Failed to open file!");
    return NULL;
  }

  /* Skip over NOAA file header */
  while (file.available()) {
    currentChar = file.read();
    if (previousChar == '\t' && currentChar == '\t') {  // Finds two tabs (between Date and Day)
      break;
    }
    previousChar = currentChar;
  }

  file.readStringUntil('\n');   // Skips to end of that line (Date, Day, Time, etc)

  /* Find closest tide time */
  while (file.available()) {
    line = file.readStringUntil('\n');  // Grab line off SD
    line.trim();
    if (line.length() > 0) {

      year = line.substring(0,4);     // Pulling all date and time info
      month = line.substring(5,7);
      day = line.substring(8,10);
      time = line.substring(15, 20);

      pred = line.substring(21, line.indexOf('\t', 21));  // Grabs tide prediction

      compDate = String(year + "-" + month + "-" + day + " " + time + ":00"); // Formats date from SD

      /* Checks dates until one is after the next drop time */
      if (strcmp(nextDropTime.substring(2).c_str(), compDate.substring(2).c_str()) <= 0) {

        /* See if the time before or after the next drop time is closer, returns the closer tide prediction */
        if (abs(makeTime(parseTime(nextDropTime.c_str())) - makeTime(parseTime(prevCompDate.c_str()))) 
              > abs(makeTime(parseTime(nextDropTime.c_str())) - makeTime(parseTime(compDate.c_str())))) {
          file.close();
          return prevPred.toDouble();   // Returns previous prediction
        } else {
          file.close();
          return pred.toDouble();       // Returns next prediction
        }

      }

      /* Saves in case next date is after the drop time to compare */
      prevPred = pred;
      prevCompDate = compDate; 
    }
  }

  /* Only reaches this section if no dates are after next drop time (something is wrong) */
  file.close();
  Serial.println("No tide prediction found.");
  return NULL;
}
