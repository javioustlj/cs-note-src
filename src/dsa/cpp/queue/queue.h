/******************************************************************************************
 * Computer Science Note Source code
 * Javious Tian, javioustlj@outlook.com
 * 夏田墨  javious_tian@zuaa.zju.edu.cn
 * Copyright (c) 2024 Javioustlj. All rights reserved.
 ******************************************************************************************/

// Intrusive Queue
template<typename T, T *T::*next>
struct IntruQueue {
  T *head, *tail;
};

