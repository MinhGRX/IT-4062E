package service.TimeDistance;

import java.time.Duration;
import java.time.LocalTime;

public class SecondDistance implements TimeDistance{
    @Override
    public long subtractTimes(String time1, String time2) {
        LocalTime localTime1 = LocalTime.parse(time1);
        LocalTime localTime2 = LocalTime.parse(time2);

        Duration duration = Duration.between(localTime2, localTime1);
        return  duration.getSeconds();
    }
}
