// #include "types/rect.h"
// #include <stdlib.h>
// #include <stdio.h>
// #include <aux_code/def.h>
// #include "math.h"

// bool operator==(const Rect& lhs,const Rect& rhs){
//     return rect_equal(&lhs,&rhs);
// }

// bool intersect_rect(const Rect* rect1, const Rect* rect2) {
// 	if(rect1 && rect2)
// 		return !(rect2->x1 > rect1->x2 ||
// 			rect2->x2 < rect1->x1 || 
// 			rect2->y1 > rect1->y2||
// 			rect2->y2 < rect1->y1);
// 	else {
// 		fprintf(stderr,"Not defined arguments");
// 		exit(1);
// 	}
// }

// bool point_in_rect(const Rect* rect, const Coord point){
// 	if(point.lat_>=rect->y2 && point.lat_<=rect->y1 && point.lon_>=rect->x1 && point.lon_<=rect->x2)
// 		return true;
// 	else return false;
// }

// Rect intersection_rect(const Rect* source, const Rect* searched){
// 	if(source && searched)
//         #ifndef __cplusplus
// 		return Rect(.x1 = max(source->x1,searched->x1), 
// 					.x2 = min(source->x2,searched->x2),
// 					.y1 = max(source->y1,searched->y1),
// 					.y2 = min(source->y2,searched->y2));
//         #else
//         return {std::max(source->x1,searched->x1), 
//                 std::min(source->x2,searched->x2),
//                 std::max(source->y1,searched->y1),
//                 std::min(source->y2,searched->y2)};
//         #endif
// 	else {
// 		fprintf(stderr,"Not defined arguments");
// 		exit(1);
// 	}
// }

// //may be extended to different coord systems (but actually WGS84)
// bool correct_rect(Rect* rect){
// 	if(is_correct_rect(rect)){
// 		if(rect->x1>rect->x2){
// 			double tmp = rect->x1;
// 			rect->x1 = rect->x2;
// 			rect->x2 = tmp;
// 		}
// 		if(rect->y1<rect->y2){
// 			double tmp = rect->y1;
// 			rect->y1 = rect->y2;
// 			rect->y2 = tmp;
// 		}
// 		return true;
// 	}
// 	else return false;
// }

// bool is_correct_rect(const Rect* rect){
// 	if(rect->x1>=0 && rect->x2<=180 && rect->y1<=90 && rect->y2>=-90)
// 		return true;
// 	else return false;
// }
// //by left-top corner
// Rect merge_rect(const Rect* r_1,const Rect* r_2){
// 	Rect res = Rect();
// 	if(is_correct_rect(r_1)){
// 		if(is_correct_rect(r_2)){
// 			if(!rect_equal(r_1,r_2)){
// 				res.x1 = r_1->x1<r_2->x1?r_1->x1:r_2->x1;
// 				res.x2 = r_1->x2>r_2->x2?r_1->x2:r_2->x2;
// 				res.y1 = r_1->y1>r_2->y1?r_1->y1:r_2->y1;
// 				res.y2 = r_1->y2<r_2->y2?r_1->y2:r_2->y2;
// 				return res;
// 			}
// 			else return *r_1;
// 		}
// 		else return *r_1; 
// 	}
// 	else{
// 		if(is_correct_rect(r_2))
// 			return *r_2;
// 	}
// 	return Rect();
// }

// bool rect_equal(const Rect* lhs,const Rect* rhs){
// 	return lhs->x1==rhs->x1 && lhs->x2==rhs->x2 && lhs->y1==rhs->y1 && lhs->y2==rhs->y2;
// }