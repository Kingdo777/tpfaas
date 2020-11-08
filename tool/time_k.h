//
// Created by kingdo on 2020/11/6.
//

#ifndef TPFAAS_TIME_K_H
#define TPFAAS_TIME_K_H

#define NS_NS  (1)
#define US_NS  (1000*NS_NS)
#define MS_NS  (1000*US_NS)
#define S_NS   (1000*MS_NS)
#define MINUTE_NS (60*S_NS)
#define HOUR_NS    (60*MINUTE_NS)

#define NS_US   0
#define US_US   (1)
#define MS_US  (1000*US_US)
#define S_US   (1000*MS_US)
#define MINUTE_US (60*S_US)
#define HOUR_US (60*MINUTE_US)

#define NS_MS  0
#define US_MS  0
#define MS_MS  (1)
#define S_MS   (1000*MS_MS)
#define MINUTE_MS (60*S_MS)
#define HOUR_MS    (60*MINUTE_MS)

#define NS_S  (0)
#define US_S  (0)
#define MS_S  (0)
#define S_S   (1)
#define MINUTE_S (60*S_S)
#define HOUR_S    (60*MINUTE_S)

#endif //TPFAAS_TIME_K_H
