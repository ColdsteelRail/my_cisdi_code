#include "muduo/base/Timestamp.h"
#include <vector>
#include <stdio.h>

using muduo::Timestamp;

void passByConstReference(const Timestamp& x)
{
  printf("%s\n", x.toString().c_str());
}

void passByValue(Timestamp x)
{
  printf("%s\n", x.toString().c_str());
}

void benchmark()
{
  const int kNumber = 1000*1000;

  std::vector<Timestamp> stamps;
  stamps.reserve(kNumber);
  for (int i = 0; i < kNumber; ++i)
  {
    stamps.push_back(Timestamp::now());
  }
  printf("%s\n", stamps.front().toString().c_str());
  printf("%s\n", stamps.back().toString().c_str());
  printf("%f\n", timeDifference(stamps.back(), stamps.front()));

  int increments[100] = { 0 };
  int64_t start = stamps.front().microSecondsSinceEpoch();
  for (int i = 1; i < kNumber; ++i)
  {
    int64_t next = stamps[i].microSecondsSinceEpoch();
    int64_t inc = next - start;
    start = next;
    if (inc < 0)
    {
      printf("reverse!\n");
    }
    else if (inc < 100)
    {
      ++increments[inc];
    }
    else
    {
      printf("big gap %d\n", static_cast<int>(inc));
    }
  }

  for (int i = 0; i < 100; ++i)
  {
    printf("%2d: %d\n", i, increments[i]);
  }
}

int main()
{
  Timestamp now(Timestamp::now());
  printf("%s\n", now.toString().c_str());
  passByValue(now);
  passByConstReference(now);
  benchmark();
}
/*
1597651698.691177
1597651698.691177
1597651698.691177
1597651698.691260
1597651698.778895
0.087635
big gap 215
big gap 382
big gap 2802
big gap 4479
big gap 14009
big gap 4036
big gap 3967
big gap 2783
big gap 8496
big gap 8551
 0: 967988
 1: 31317
 2: 318
 3: 31
 4: 7
 5: 4
 6: 6
 7: 1
 8: 0
 9: 0
10: 2
11: 1
12: 25
13: 97
14: 7
15: 16
16: 12
17: 2
18: 48
19: 34
20: 20
21: 10
22: 0
23: 3
24: 0
25: 2
26: 5
27: 3
28: 8
29: 6
30: 1
31: 2
32: 0
33: 4
34: 1
35: 0
36: 1
37: 1
38: 0
39: 0
40: 0
41: 0
42: 0
43: 0
44: 0
45: 0
46: 0
47: 0
48: 0
49: 0
50: 0
51: 0
52: 0
53: 1
54: 0
55: 0
56: 0
57: 0
58: 0
59: 0
60: 0
61: 0
62: 0
63: 0
64: 0
65: 0
66: 0
67: 1
68: 0
69: 0
70: 1
71: 1
72: 0
73: 0
74: 0
75: 0
76: 0
77: 0
78: 0
79: 0
80: 0
81: 0
82: 0
83: 0
84: 1
85: 0
86: 0
87: 0
88: 0
89: 0
90: 0
91: 0
92: 0
93: 0
94: 0
95: 0
96: 0
97: 0
98: 1
99: 0
*/