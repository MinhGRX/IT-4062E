package service.TimeFormatter;

import service.TimeFormatter.TimeFormatter;

import java.time.LocalTime;
import java.time.format.DateTimeFormatter;

public class HHMMSSFormatter implements TimeFormatter {
    @Override
    public String getCurrentTime() {
        LocalTime currentTime = LocalTime.now();
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("HH:mm:ss");
        return currentTime.format(formatter);
    }
}
