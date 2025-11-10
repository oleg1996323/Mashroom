#include "sections/grid/def.h"
// #include "sections/binary/def.h"
// #include <stdio.h>


template<>
std::expected<ScanMode,std::exception> from_json(const boost::json::value& val){
    if(val.is_object()){
		ScanMode result;
		auto& obj = val.as_object();
		if(obj.contains("scan -i direction") && obj.at("scan -i direction").is_bool())
		    result.points_sub_i_dir = obj.at("scan -i direction").as_bool();
		else std::unexpected(std::invalid_argument("invalid JSON input"));
		if(obj.contains("scan +j direction") && obj.at("scan +j direction").is_bool())
		    result.points_sub_j_dir = obj.at("scan +j direction").as_bool();
		else std::unexpected(std::invalid_argument("invalid JSON input"));
		if(obj.contains("adjacent consec j-dir pts") && obj.at("adjacent consec j-dir pts").is_bool())
		    result.adj_points_j_dir = obj.at("adjacent consec j-dir pts").as_bool();
		else std::unexpected(std::invalid_argument("invalid JSON input"));
		return result;
	}
	else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const ScanMode& val){
	boost::json::object obj;
	obj["scan -i direction"].as_bool() = val.points_sub_i_dir;
	obj["scan +j direction"].as_bool() = val.points_sub_j_dir;
	obj["adjacent consec j-dir pts"].as_bool() = val.adj_points_j_dir;
	return obj;
}

template<>
std::expected<ResolutionComponentFlags,std::exception> from_json(const boost::json::value& val){
    if(val.is_object()){
		ResolutionComponentFlags result;
		auto& obj = val.as_object();
		if(obj.contains("has direction") && obj.at("has direction").is_bool())
		    result.given_direction = obj.at("scan -i direction").as_bool();
		else std::unexpected(std::invalid_argument("invalid JSON input"));
		if(obj.contains("has direction") && obj.at("has direction").is_bool())
		    result.earth_spheroidal = obj.at("has direction").as_bool();
		else std::unexpected(std::invalid_argument("invalid JSON input"));
		if(obj.contains("UV-comp grid direction") && obj.at("UV-comp grid direction").is_bool())
		    result.grid_direction_uv_comp = obj.at("UV-comp grid direction").as_bool();
		else std::unexpected(std::invalid_argument("invalid JSON input"));
		return result;
	}
	else return std::unexpected(std::invalid_argument("invalid JSON input"));
}

template<>
boost::json::value to_json(const ResolutionComponentFlags& val){
	boost::json::object obj;
	obj["has direction"] = val.given_direction;
	obj["earth sphere"] = val.earth_spheroidal;
	obj["UV-comp grid direction"] = val.grid_direction_uv_comp;
	return obj;
}

// int GDS_grid(
//    unsigned char *gds, 
//    unsigned char *bds, 
//    int *nx, 
//    int *ny, 
//    long int *nxny) {

// int i, d, ix, iy, pl;
// long int isum;

// *nx = ix = GDS_LatLon_nx(gds);
// *ny = iy = GDS_LatLon_ny(gds);
// *nxny = ix * iy;

// /* thin grid */

// if (GDS_Gaussian(gds) || GDS_LatLon(gds)) {
// if (ix == 65535) {
// *nx = -1;
// /* reduced grid */
// isum = 0;
// pl = GDS_PL(gds);
// for (i = 0; i < iy; i++) {
// isum += gds[pl+i*2]*256 + gds[pl+i*2+1];
// }
// *nxny = isum;
// }
// return 0;
// }
// if (GDS_Triangular(gds)) {
// i = GDS_Triangular_ni(gds);
// d = GDS_Triangular_nd(gds);
// *nx = *nxny = d * (i + 1) * (i + 1);
// *ny = 1;
// return 0;
// }
// if (GDS_Harmonic(gds)) {
// if (BDS_ComplexPacking(bds)) {
// *nx = BDS_NValues(bds);
// *ny = -1;
// }
// else {
// /* this code assumes j, k, m are consistent with bds */
//    *nx = *nxny = (8*(BDS_LEN(bds)-15)-BDS_UnusedBits(bds))/
// BDS_NumBits(bds)+1;
//    if ((8*(BDS_LEN(bds)-15)-BDS_UnusedBits(bds)) % BDS_NumBits(bds)) {
//   fprintf(stderr,"inconsistent harmonic BDS\n");
//    }
//    *ny = 1;
// }
// }
// return 0;
// }

// void GDS_prt_thin_lon(unsigned char *gds) {
// int iy, i, col, pl;

// iy = GDS_LatLon_ny(gds);
// iy = (iy + 1) / 2;
// iy = GDS_LatLon_ny(gds);

// if ((pl = GDS_PL(gds)) == -1) {
// fprintf(stderr,"\nprogram error: GDS_prt_thin\n");
// return;
// }
// for (col = i = 0; i < iy; i++) {
// if (col == 0) printf("   ");
// printf("%5d", (gds[pl+i*2] << 8) + gds[pl+i*2+1]);
// col++;
// if (col == NCOL) {
// col = 0;
// printf("\n");
// }
// }
// if (col != 0) printf("\n");
// }

// void GDS_winds(unsigned char *gds, int verbose) {
// int scan = -1, mode = -1;

// if (gds != NULL) {
// if (GDS_LatLon(gds)) {
// scan = GDS_LatLon_scan(gds);
// mode = GDS_LatLon_mode(gds);
// }
// else if (GDS_Mercator(gds)) {
// scan =GDS_Merc_scan(gds);
// mode =GDS_Merc_mode(gds);
// }
// /* else if (GDS_Gnomonic(gds)) { */
// else if (GDS_Lambert(gds)) {
// scan = GDS_Lambert_scan(gds);
// mode = GDS_Lambert_mode(gds);
// }
// else if (GDS_Gaussian(gds)) {
// scan = GDS_LatLon_scan(gds);
// mode = GDS_LatLon_mode(gds);
// }
// else if (GDS_Polar(gds)) {
// scan = GDS_Polar_scan(gds);
// mode = GDS_Polar_mode(gds);
// }
// else if (GDS_RotLL(gds)) {
// scan = GDS_RotLL_scan(gds);
// mode = GDS_RotLL_mode(gds);
// }
// /* else if (GDS_Triangular(gds)) { */
// else if (GDS_ssEgrid(gds)) {
// scan = GDS_ssEgrid_scan(gds);
// mode = GDS_ssEgrid_mode(gds);
// }
// else if (GDS_fEgrid(gds)) {
// scan = GDS_fEgrid_scan(gds);
// mode = GDS_fEgrid_mode(gds);
// }
// else if (GDS_ss2dEgrid(gds)) {
// scan = GDS_ss2dEgrid_scan(gds);
// mode = GDS_ss2dEgrid_mode(gds);
// }
// else if (GDS_ss2dBgrid(gds)) {
//   scan = GDS_ss2dBgrid_scan(gds);
//   mode = GDS_ss2dBgrid_mode(gds); 
// }
// }
// if (verbose == 1) {
// if (mode != -1) {
// if (mode & 8) printf("winds in grid direction:");
// else printf("winds are N/S:"); 
// }
// }
// else if (verbose == 2) {
// if (scan != -1) {
// printf(" scan: %s", scan_mode[(scan >> 5) & 7]);
// }
// if (mode != -1) {
// if (mode & 8) printf(" winds(grid) ");
// else printf(" winds(N/S) "); 
// }
// }
// }