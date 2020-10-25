# ETL Process

<!-- ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ -->

<!-- ## 作業說明：

- 中文：https://hackmd.io/@dsclab/rktgrMOUD
- 英文：https://hackmd.io/@dsclab/B1cxhvp8w

--- -->

## 基本資料

| 學號      | 姓名   | 系級     |
| --------- | ------ | -------- |
| E14066282 | 溫梓傑 | 機械 110 |

---

:::info

## 開發環境

- **OS:** Ubuntu 18.04.4 LTS
- **CPU:** Intel® Core™ i5-6300HQ CPU @ 2.30GHz × 4
- **Memory:** 7.7 GiB
- **Swap:** 1.37GiB
- **Language:** C++ (standard 11)
- **Compiler:** g++ (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
  :::

---

## 程式架構簡介

![](https://i.imgur.com/DWvBdYL.png)

## 程式執行時間

### Conversion.cpp

| thread number | execution time |
| :------------ | -------------- |
| 1             | 72,461,095 ms  |
| 2             | 61,880,589 ms  |
| 3             | 62,515,777 ms  |
| 4             | 77,312,430 ms  |
| 5             | 70,012,804 ms  |
| 6             | 74,730,064 ms  |
| 7             | 76,904,779 ms  |
| 8             | 81,442,807 ms  |
| 9             | 79,445,625 ms  |
| 10            | 77,947,161 ms  |
| 11            | 80,149,431 ms  |
| 12            | 85,100,788 ms  |
| 13            | 88,232,747 ms  |
| 14            | 87,634,307 ms  |
| 15            | 98,887,928 ms  |
| 16            | 101,510,454 ms |
| 17            | 99,407,615 ms  |
| 18            | 99,020,348 ms  |
| 19            | 94,580,918 ms  |
| 20            | 104,172,539 ms |
| 100           | 146,814,795 ms |

![](https://i.imgur.com/NCc2pPp.png)
::: success
**[圖一]CPU time v.s. Thread number**
可以發現在 thread number = 2 時，CPU time 最小，並且在[圖一]中，大致上呈現線性分佈。
:::

## 程式開發與使用說明

### How to run code

```console=1
# Compile
$ make
# Run
$ ./converter.out [thread]
```

---

## 效能分析報告

#### 程式執行期間各個 Stage 對電腦資源使用情形

![](https://i.imgur.com/soPouNd.jpg)
![](https://i.imgur.com/IRGf6eo.png)
:::warning
**[圖三]、[圖四] 不同階段使用硬體資源之狀況**

| Stage               | CPU                                   | Memory                                 | I/O        |
| :------------------ | ------------------------------------- | -------------------------------------- | ---------- |
| 1. 分析資料         | low usage                             | low usage                              | 500 MB/sec |
| 2. 平行輸出至暫存檔 | high usage 因 conversion 須做字串處理 | low usage 因使用 limited buffer 做讀寫 | 10 MB/sec  |
| 3. 平行合併檔案     | low usage                             | low usage 因使用 limited buffer 做讀寫 | 457 MB/sec |

:::

#### 結論

從以上實驗可以看出，作業系統在背後做了非常多優化服務，包括 CPU 資源分配(即行程管理)、記憶體管理、虛擬記憶體管理、Disk I/O 管理等。
目前完成兩個作業後，我發現最大的困難總是在 I/O 處理速度，程式的平行度再高，也無法克服這個障礙，因此我認為，作業系統或許在 I/O 資源分配時，能夠提供一些 API 來設定優先度等等細節處理。

### 改善之處

由於此程式是假設 CSV 檔案大小可能超過記憶體大小下去設計的，因此限縮了記憶體使用資源，需要依靠大量的 IO 讀寫，而 IO 讀寫正是此程式的最大瓶頸。
在此提出一些可以改善之處：

1. 可以將程式設定成兩種模式，分別為大檔模式與小檔模式，於偵測檔案大小後做出判斷。
   - 大檔模式：限制 Memory 資源(過程參考上述程式架構)
   - 小檔模式：把全部資料讀進記憶體，再進行單一 thread 輸出
2. 對於"平行輸出至暫存檔"階段再進行優化，因為他是整個程式中執行最慢的部份(雖然我已經盡力ㄌ QQ)。

## Contact

### :email: e14066282@gs.ncku.edu.tw
