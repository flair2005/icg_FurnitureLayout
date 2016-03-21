#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 600
#define WINDOW_TITLE  "test"
#define CLEAR_COLOR		Tcolor4<float>(1.0, 1.0, 1.0, 1.0)
#define WHITE			Tcolor3<float>(1.0, 1.0, 1.0)
#define ORANGE			Tcolor3<float>(234 / 255.0, 158 / 255.0, 0.0 / 255.0)
#define BLUE			Tcolor3<float>(0.0 / 256.0, 102 / 256.0, 165.0 / 256.0)
#define RED				Tcolor3<float>(196 / 256.0, 26.0 / 256.0, 65.0 / 256.0)
#define GREEN			Tcolor3<float>(160.0 / 256.0, 202.0 / 256.0, 90.0 / 256.0)
#define WALLCOLOR		Tcolor3<float>(0.588, 0.6, 0.59)
#define FLOORCOLOR		Tcolor3<float>(0.82, 0.71, 0.54)
#define TABLECOLOR		Tcolor3<float>(0.48, 0.26, 0.20)
#define DININGCHAIRCOLOR	Tcolor3<float>(28 / 256.0, 80 / 256.0, 147 / 256.0)
#define SOFACOLOR		Tcolor3<float>(0.81, 0.33, 0.41)

//床の領域
#define floorScale 230.0  //家具4つのとき230.0がいいのかな？ book増やすと280？？
#define MARGIN 120.0
#define wideFloorScale 230.0
#define depthFloorScale 150.0
//人の領域
#define PersonArea 20.0






#endif
