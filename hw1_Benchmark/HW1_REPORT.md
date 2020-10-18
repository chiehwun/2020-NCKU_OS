# Benchmark

<!-- ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ -->

## 作業說明：

- 中文：https://hackmd.io/@dsclab/r1viIcBSw
- 英文：https://hackmd.io/@dsclab/H11Lu3FrP

---

## 基本資料

| 學號      | 姓名   | 系級     |
| --------- | ------ | -------- |
| E14066282 | 溫梓傑 | 機械 110 |

---

## 開發環境

- **OS:** Ubuntu 18.04.4 LTS
- **CPU:** Intel® Core™ i5-6300HQ CPU @ 2.30GHz × 4
- **Memory:** 7.7 GiB
- **Swap:** 1.37GiB
---

## 程式架構簡介

:::info

### 1. generate(): 測資產生程式

### 2. External Sorting 程式樹狀圖

- **CreateInitRuns()**：讀取 `input.txt` 產生初始行程
  - **rMergeSort()**：遞迴法合併排序(產生 linka)
  - **List1()**：使用 linka 搭配 ListSort 完成記憶體內的排序
- **MergeRuns()**：將所有 run，透過 K-way 合併演算法，完成外部排序
  :::

## 程式執行時間

### 測資產生程式

| 1GiB  | 4GiB    | 8GiB    | 16GiB   |
| :---- | ------- | ------- | ------- |
| 20 sec | 93 sec | 194 sec | 395 sec |

### 外部排序程式

|                      | 1GiB | 4GiB    | 8GiB     | 16GiB    |
| :------------------- | ---- | ------- | -------- | -------- |
| CreateInitRuns()     |161 sec| 725 sec | 1362 sec | 2863 sec|
| MergeRuns()          |40 sec | 208 sec | 399 sec  | 813 sec|
| Total execution time | 201 sec| 933 sec | 1762 sec | 3676 sec|

## 程式開發與使用說明

:::info

- **Language:** C/C++
- **Compiler:** g++ (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
  :::
  :::danger

### Before Compiling the Code

1. 確認 Project 目錄下存在以下兩個子目錄
   - `/obj`
   - `/test_files`
2. 請將自訂測資 `input.txt` 複製至 `/test_files`
   :::

### How to run code

```console
# Compile
hello@example:~$ make
# Run
hello@example:~$ ./ext_sort -s  #[-arguments]
```

> **參數說明**
>
> - _-s:_ 開始排序
> - _-g:_ 產生與記憶體大小相等之測資(**舊測資將被覆寫**)
> - _-i:_ 查看記憶體大小

---

## 效能分析報告

### 排序程式對硬體效能優化

#### Ⅰ. I/O 部份

1. 輸出檔案時，使用 **Buffer** 加速輸出
   I/O 途徑：Memory -> CPU -> Disk
   測試程式：CreateInitRuns()
   |不使用 Buffer|使用 Buffer |
   |:-----------|------------|
   | 12.86 MiB/s | 78.13 MiB/s |
   **實測效果十分顯著！**

2. 使用不同 library 之速度差異
   I/O 途徑：CPU -> Memory -> CPU -> Disk
   測試程式：generate (測資產生程式)
   | C: <stdio.h> |C++: <fstream> |
   |:----------|--------------------|
   |12.13 MiB/s |10.23 MiB/s |
   **實測效果不明顯，但速度有微幅提昇**
   注：後來有針對generate()做優化，速度可以接近 52 MiB/s

3. 使用 K-way Merge 方法
   理論上適當的 K 值能夠使程式執行間縮短，因為當 K 越大，I/O 傳輸的總量會跟著下降，而外部排序程式最慢的部份在於 I/O 的資料傳輸。
   以下為實驗結果：
   測資使用： 8GB 分割成 4 runs 作為初始行程

| K-way                             | 2    | 3   | 4   |
| :-------------------------------- | ---- | --- | --- |
| MergeRuns(): execution time (sec) | 1064 | 990 | 464 |

**速度有顯著提昇**

#### Ⅱ. 系統效能的觀察

##### 1. 單一程式運行

- CPU 方面
  最耗 CPU 的函式為 rMergeSort()與 List1()_(見圖一)_，它們可以把 CPU 4 核心中其中 1 核吃到 100%  _(見圖二 核心 4)_
  ![](https://i.imgur.com/9S6XJie.jpg)
  圖一
- Memory 方面
  rMergeSort()、List1()由於要做內部排序，因此會消耗大量記憶體。
  _(見圖二 MEM%欄位)_
  ![](https://i.imgur.com/idG70n6.jpg)
  圖二
- I/O 方面
  ![](https://i.imgur.com/yvQcvJT.jpg)
  圖三
  I/O 本身不需要用到大量記憶體，在此程測試函式 MergeRuns()中，輸出是採用 100MiB 大小的 char\* 作為 Buffer，使用 Buffer 能加速的原因是因為 CPU 存取速度遠比 Disk 快，若引入存取速度介於中間的 RAM(即 Buffer)，能夠作為緩衝，提升運行速度。

##### 2. 多重程式運行

- 程式限制
  目前單一程式執行的記憶體使用策略是，去`/proc/meminfo`中抓取"MemFree:"欄位，將 MemFree 的記憶體空間乘以一個安全倍率(<1.0)，作為該程式所配置的記憶體大小。
- 執行多重程式之方法
  複製整套程式至另一目錄，調整安全倍率使多支程式能夠平分記憶體。其餘系統資源例如 CPU、Disk I/O 交給作業系統管理。
- 實驗觀察結果

  | 1GiB/750 MiB RAM/2 runs/2 way | thread0 | thread1 | thread2 | thread3 |
  | :---------------------------- | ------- | ------- | ------- | ------- |
  | CreateInitRuns()              | 219 sec | 241 sec | 239 sec | 238 sec |
  | MergeRuns()                   | 108 sec | 101 sec | 103 sec | 103 sec |
  | Total execution time          | 328 sec | 342 sec | 341 sec | 341 sec |

  - CPU
    ![](https://i.imgur.com/f6mmZHQ.jpg)
    圖四 4 個 thread 全部吃 100% CPU
    ![](https://i.imgur.com/LBqNJNA.jpg)
    圖五 ListMerge 為最大瓶頸
  - Memory
    就如同前述，我的記憶體管理是手動調控的，因此每支程式平均分擔 MemFree 之記憶體容量 _(見圖四)_

  - I/O

  | I/O Result | thread0 | thread1 | thread2 | thread3 | Total |
  | :--------- | ------- | ------- | ------- | ------- | ----- |
  | MiB/s      | 27.50   | 19.72   | 19.90   | 19.61   | 86.73 |

  作業系統將 I/O 的速度"大略"平均分配給每支程式，其中 thread0 分配較多的資源可能是因為它比較早執行(我的程式啟動順序是 0-1-2-3)。

##### 3.結論

從以上實驗可以看出，作業系統在背後做了非常多優化服務，包括 CPU 資源分配(即行程管理)、記憶體管理、虛擬記憶體管理、Disk I/O 管理等。

## Contact

### :email: e14066282@gs.ncku.edu.tw

## 附錄

- 多重程式執行結果(4 threads)

```console
wun@wun-linux /media/wun/OS/2020-NCKU_OS/MultiThread_test/thread0
 % make && ./ext_sort0 -s
make: Nothing to be done for 'all'.
MemTotal:             8,033,332	kB
MemFree:                624,084	kB
-s

===================== START SORTING =====================
FOPEN_MAX = 16
"input_1GiB.txt" size:      1,080,032,575 Bytes

================== Create Initial Runs ==================
BF_CNT:              65,536,000
FREE_MEM:               624,084	KB
*sort_list MEM:     262,144,000	Bytes
Input to sort_list: run0:
[==================================================] 65536000/65536000 100.00 %
 0-#sort_list: 	65536000	sorting...linkb created...ok!

rMergeSort(): execution time = 	115,410,249	ms
Write "0.run"
[==================================================] 65536000/65536000 100.00 %
Writes to run: execution time = 	26,176,221	ms
Writing avg. speed = 		27.50	MB/sec
Successfully write! "0.run" runLine: 65536000

Input to sort_list: run1:
[=========================>                        ] 32804334/65536000 50.06 %
 1-#sort_list: 	32804334	sorting...linkb created...ok!

rMergeSort(): execution time = 	42,875,887	ms
Write "1.run"
[==================================================] 32804334/32804334 100.00 %
Writes to run: execution time = 	8,203,315	ms
Writing avg. speed = 		43.92	MB/sec
Successfully write! "1.run" runLine: 32804334

Successfully delete[] buf_out
Successfully delete[] sort_list

CreateInitRuns(): execution time = 	219,224,224	ms

================== Merge Run Files ==================
 0  1
k: 2, Kway: 2, FOPEN_MAX-1: 15
Write "output.txt": k=2

[=================================================>] 98340230/98340332 100.00 %
MergeRuns(Kway=2): execution time = 	108,460,124	ms

====================== END SORTING ======================

ext_merge_sort(): execution time = 	327,684,421	ms
```

```console
wun@wun-linux /media/wun/OS/2020-NCKU_OS/MultiThread_test/thread1
 %  make && ./ext_sort1 -s
g++ -std=c++11 -Wall -o obj/ext_sort1.o -c src/ext_sort1.cpp
g++ -std=c++11 -Wall -o ext_sort1 obj/ext_sort1.o obj/Clock.o
MemTotal:             8,033,332	kB
MemFree:                242,396	kB
-s

===================== START SORTING =====================
FOPEN_MAX = 16
"input_1GiB.txt" size:      1,080,032,575 Bytes

================== Create Initial Runs ==================
BF_CNT:              65,536,000
FREE_MEM:               242,396	KB
*sort_list MEM:     262,144,000	Bytes
Input to sort_list: run0:
[==================================================] 65536000/65536000 100.00 %
 0-#sort_list: 	65536000	sorting...linkb created...ok!

rMergeSort(): execution time = 	117,868,498	ms
Write "0.run"
[==================================================] 65536000/65536000 100.00 %
Writes to run: execution time = 	36,507,944	ms
Writing avg. speed = 		19.72	MB/sec
Successfully write! "0.run" runLine: 65536000

Input to sort_list: run1:
[=========================>                        ] 32804334/65536000 50.06 %
 1-#sort_list: 	32804334	sorting...linkb created...ok!

rMergeSort(): execution time = 	46,384,436	ms
Write "1.run"
[==================================================] 32804334/32804334 100.00 %
Writes to run: execution time = 	18,336,201	ms
Writing avg. speed = 		19.65	MB/sec
Successfully write! "1.run" runLine: 32804334

Successfully delete[] buf_out
Successfully delete[] sort_list

CreateInitRuns(): execution time = 	240,541,457	ms

================== Merge Run Files ==================
 0  1
k: 2, Kway: 2, FOPEN_MAX-1: 15
Write "output.txt": k=2

[=================================================>] 98340230/98340332 100.00 %
MergeRuns(Kway=2): execution time = 	101,778,174	ms

====================== END SORTING ======================

ext_merge_sort(): execution time = 	342,320,211	ms
```

```console
wun@wun-linux /media/wun/OS/2020-NCKU_OS/MultiThread_test/thread2
 %  make && ./ext_sort2 -s
g++ -std=c++11 -Wall -o obj/ext_sort2.o -c src/ext_sort2.cpp
g++ -std=c++11 -Wall -o ext_sort2 obj/ext_sort2.o obj/Clock.o
MemTotal:             8,033,332	kB
MemFree:                180,520	kB
-s

===================== START SORTING =====================
FOPEN_MAX = 16
"input_1GiB.txt" size:      1,080,032,575 Bytes

================== Create Initial Runs ==================
BF_CNT:              65,536,000
FREE_MEM:               180,520	KB
*sort_list MEM:     262,144,000	Bytes
Input to sort_list: run0:
[==================================================] 65536000/65536000 100.00 %
 0-#sort_list: 	65536000	sorting...linkb created...ok!

rMergeSort(): execution time = 	116,122,918	ms
Write "0.run"
[==================================================] 65536000/65536000 100.00 %
Writes to run: execution time = 	36,160,826	ms
Writing avg. speed = 		19.90	MB/sec
Successfully write! "0.run" runLine: 65536000

Input to sort_list: run1:
[=========================>                        ] 32804334/65536000 50.06 %
 1-#sort_list: 	32804334	sorting...linkb created...ok!

rMergeSort(): execution time = 	46,047,887	ms
Write "1.run"
[==================================================] 32804334/32804334 100.00 %
Writes to run: execution time = 	17,506,661	ms
Writing avg. speed = 		20.58	MB/sec
Successfully write! "1.run" runLine: 32804334

Successfully delete[] buf_out
Successfully delete[] sort_list

CreateInitRuns(): execution time = 	238,515,359	ms

================== Merge Run Files ==================
 0  1
k: 2, Kway: 2, FOPEN_MAX-1: 15
Write "output.txt": k=2

[=================================================>] 98340230/98340332 100.00 %
MergeRuns(Kway=2): execution time = 	102,974,675	ms

====================== END SORTING ======================

ext_merge_sort(): execution time = 	341,490,237	ms
```

```console
wun@wun-linux /media/wun/OS/2020-NCKU_OS/MultiThread_test/thread3
 %  make && ./ext_sort3 -s
g++ -std=c++11 -Wall -o obj/ext_sort3.o -c src/ext_sort3.cpp
g++ -std=c++11 -Wall -o ext_sort3 obj/Clock.o obj/ext_sort3.o
MemTotal:             8,033,332	kB
MemFree:                145,056	kB
-s

===================== START SORTING =====================
FOPEN_MAX = 16
"input_1GiB.txt" size:      1,080,032,575 Bytes

================== Create Initial Runs ==================
BF_CNT:              65,536,000
FREE_MEM:               145,056	KB
*sort_list MEM:     262,144,000	Bytes
Input to sort_list: run0:
[==================================================] 65536000/65536000 100.00 %
 0-#sort_list: 	65536000	sorting...linkb created...ok!

rMergeSort(): execution time = 	116,452,379	ms
Write "0.run"
[==================================================] 65536000/65536000 100.00 %
Writes to run: execution time = 	36,706,883	ms
Writing avg. speed = 		19.61	MB/sec
Successfully write! "0.run" runLine: 65536000

Input to sort_list: run1:
[=========================>                        ] 32804334/65536000 50.06 %
 1-#sort_list: 	32804334	sorting...linkb created...ok!

rMergeSort(): execution time = 	46,447,135	ms
Write "1.run"
[==================================================] 32804334/32804334 100.00 %
Writes to run: execution time = 	17,616,614	ms
Writing avg. speed = 		20.45	MB/sec
Successfully write! "1.run" runLine: 32804334

Successfully delete[] buf_out
Successfully delete[] sort_list

CreateInitRuns(): execution time = 	238,854,269	ms

================== Merge Run Files ==================
 0  1
k: 2, Kway: 2, FOPEN_MAX-1: 15
Write "output.txt": k=2

[=================================================>] 98340230/98340332 100.00 %
MergeRuns(Kway=2): execution time = 	102,569,851	ms

====================== END SORTING ======================

ext_merge_sort(): execution time = 	341,424,170	ms
```
