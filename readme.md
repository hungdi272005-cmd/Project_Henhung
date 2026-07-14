# 🎮 Tetris Game - STM32F429I Discovery


🔗 **Github:** [github.com/hungdi272005-cmd/Project_Henhung](https://github.com/hungdi272005-cmd/Project_Henhung)
🎥 **Video Demo:** [Google Drive - Demo Folder](https://drive.google.com/drive/folders/1Apxqqg63PXrpLm8YypJ0Wgp4oH76XWec)


Dự án triển khai trò chơi Xếp gạch (Tetris) kinh điển trên vi điều khiển STM32F429ZI (Kit phát triển STM32F429I-DISCO_REV_D01). Giao diện đồ họa được thiết kế và tối ưu hóa bằng TouchGFX, kết hợp với hệ điều hành thời gian thực FreeRTOS để xử lý mượt mà các luồng dữ liệu, nút bấm vật lý và hiệu ứng âm thanh.


## ✨ Tính năng nổi bật


- **Gameplay hoàn chỉnh**: Đầy đủ các cơ chế sinh khối ngẫu nhiên (chống lặp block), di chuyển (trái/phải), xoay khối (chống kẹt tường), rơi nhanh và xóa hàng ăn điểm.
- **Lưu trữ Kỷ lục (High Score)**: Tích hợp ghi đè an toàn vào bộ nhớ Flash của chip. Điểm "Best" sẽ luôn được lưu giữ ngay cả khi mất điện hoặc nhấn nút Reset.
- **Hiệu ứng âm thanh (Active Buzzer)**: Các âm điệu "Tinh tinh" (Bắt đầu game) và "Tình tình" (Game Over) được xử lý đồng bộ theo khung hình đồ họa, không dùng HAL_Delay, đảm bảo game không bị giật lag (Non-blocking).
- **Đồ họa Full-viền**: Bàn cờ được mở rộng lên không gian 12 cột x 16 hàng, tối ưu hóa hiển thị trên màn hình LCD 320x240 (16bpp).
- **Điều khiển cơ học**: Sử dụng các nút bấm ngoài với cơ chế chống rung phím (Debounce) được xử lý trực tiếp trong luồng Task của FreeRTOS.


## 🔌 Sơ đồ đấu nối phần cứng (Pinout)


Để chơi được game, bạn cần kết nối bo mạch với 4 nút bấm nhấn nhả và 1 còi chủ động (Active Buzzer) theo sơ đồ sau:


| Chức năng        | Chân (Pin)      | Cấu hình trong CubeMX    |
| :--------------- | :-------------- | :----------------------- |
| Nút Sang Trái    | PB12            | GPIO_Input (Pull-up)     |
| Nút Sang Phải    | PB13            | GPIO_Input (Pull-up)     |
| Nút Xoay Gạch    | PG2             | GPIO_Input (Pull-up)     |
| Nút Rơi Nhanh    | PG3             | GPIO_Input (Pull-up)     |
| Còi báo (Buzzer) | PA1 (hoặc PG13) | GPIO_Output (No pull-up) |


_Lưu ý: Các nút bấm cần được đấu xuống Mass (GND). Khi nhấn nút, trạng thái chân sẽ kéo xuống mức LOW (GPIO_PIN_RESET)._


## 🛠️ Môi trường và Công cụ phát triển


- **Phần cứng**: Bo mạch STM32F429I-DISCO (Rev D01)
- **IDE Khuyên dùng**: STM32CubeIDE hoặc VS Code. (Có thể thay đổi bằng cách mở file `.ioc` bằng STM32CubeMX và chọn lại IDE hỗ trợ như EWARM, MDK-ARM).
- **Thiết kế UI**: TouchGFX Designer.
- **Nạp code**: Tích hợp sẵn thông qua GCC và STM32CubeProgrammer (yêu cầu cài đặt ST-Link Driver).


## 📊 Chẩn đoán hiệu suất đồ họa (Performance Testing)


Dự án vẫn giữ nguyên các cấu hình chân chẩn đoán hiệu suất mặc định của TouchGFX Board Support (TBS). Bạn có thể dùng máy hiện sóng (Oscilloscope) để đo các tín hiệu sau:


- **PE2**: VSYNC_FREQ (Tần số quét màn hình)
- **PE3**: RENDER_TIME (Thời gian render đồ họa)
- **PE4**: FRAME_RATE (Tốc độ khung hình/giây)
- **PE5**: MCU_ACTIVE (Thời gian MCU hoạt động)


## 🚀 Hướng dẫn Build và Nạp Code


1. Clone repository hoặc tải thư mục dự án về máy.
2. Mở file `.project` bằng STM32CubeIDE (hoặc mở toàn bộ thư mục bằng VS Code).
3. Nhấn Build (`Ctrl + B`) để biên dịch hệ thống.
4. Kết nối mạch STM32F429I-DISCO với máy tính qua cổng Mini-USB (Cổng ST-LINK).
5. Nhấn Run (mũi tên xanh) để nạp Firmware xuống mạch. Màn hình sẽ sáng lên và sẵn sàng trải nghiệm!



