package service;

import service.TimeDistance.SecondDistance;
import service.TimeDistance.TimeDistance;
import service.TimeFormatter.HHMMSSFormatter;
import service.TimeFormatter.TimeFormatter;


public class CurrentTimeService {
    private static final CurrentTimeService currentTimeService = new CurrentTimeService();
    private CurrentTimeService() {
    }
    public String getCurrentTime() {
        return selectTimeFormatter(new HHMMSSFormatter());
    }
    private String selectTimeFormatter(TimeFormatter timeFormatter){
        return timeFormatter.getCurrentTime();
    }
    public long subtractTimes(String time1, String time2) {
        return selectTimeDistance(new SecondDistance(),time1,time2);
    }
    private long selectTimeDistance(TimeDistance timeDistance, String time1, String time2){
        return timeDistance.subtractTimes(time1,time2);
    }

    public static CurrentTimeService getInstance(){
        return currentTimeService;
    }
}
