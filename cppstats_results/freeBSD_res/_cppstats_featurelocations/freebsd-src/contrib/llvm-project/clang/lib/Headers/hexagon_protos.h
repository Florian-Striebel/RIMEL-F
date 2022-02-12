











#if !defined(__HEXAGON_PROTOS_H_)
#define __HEXAGON_PROTOS_H_ 1








#define Q6_R_abs_R __builtin_HEXAGON_A2_abs








#define Q6_P_abs_P __builtin_HEXAGON_A2_absp








#define Q6_R_abs_R_sat __builtin_HEXAGON_A2_abssat








#define Q6_R_add_RR __builtin_HEXAGON_A2_add








#define Q6_R_add_RhRh_s16 __builtin_HEXAGON_A2_addh_h16_hh








#define Q6_R_add_RhRl_s16 __builtin_HEXAGON_A2_addh_h16_hl








#define Q6_R_add_RlRh_s16 __builtin_HEXAGON_A2_addh_h16_lh








#define Q6_R_add_RlRl_s16 __builtin_HEXAGON_A2_addh_h16_ll








#define Q6_R_add_RhRh_sat_s16 __builtin_HEXAGON_A2_addh_h16_sat_hh








#define Q6_R_add_RhRl_sat_s16 __builtin_HEXAGON_A2_addh_h16_sat_hl








#define Q6_R_add_RlRh_sat_s16 __builtin_HEXAGON_A2_addh_h16_sat_lh








#define Q6_R_add_RlRl_sat_s16 __builtin_HEXAGON_A2_addh_h16_sat_ll








#define Q6_R_add_RlRh __builtin_HEXAGON_A2_addh_l16_hl








#define Q6_R_add_RlRl __builtin_HEXAGON_A2_addh_l16_ll








#define Q6_R_add_RlRh_sat __builtin_HEXAGON_A2_addh_l16_sat_hl








#define Q6_R_add_RlRl_sat __builtin_HEXAGON_A2_addh_l16_sat_ll








#define Q6_R_add_RI __builtin_HEXAGON_A2_addi








#define Q6_P_add_PP __builtin_HEXAGON_A2_addp








#define Q6_P_add_PP_sat __builtin_HEXAGON_A2_addpsat








#define Q6_R_add_RR_sat __builtin_HEXAGON_A2_addsat








#define Q6_P_add_RP __builtin_HEXAGON_A2_addsp








#define Q6_R_and_RR __builtin_HEXAGON_A2_and








#define Q6_R_and_RI __builtin_HEXAGON_A2_andir








#define Q6_P_and_PP __builtin_HEXAGON_A2_andp








#define Q6_R_aslh_R __builtin_HEXAGON_A2_aslh








#define Q6_R_asrh_R __builtin_HEXAGON_A2_asrh








#define Q6_R_combine_RhRh __builtin_HEXAGON_A2_combine_hh








#define Q6_R_combine_RhRl __builtin_HEXAGON_A2_combine_hl








#define Q6_R_combine_RlRh __builtin_HEXAGON_A2_combine_lh








#define Q6_R_combine_RlRl __builtin_HEXAGON_A2_combine_ll








#define Q6_P_combine_II __builtin_HEXAGON_A2_combineii








#define Q6_P_combine_RR __builtin_HEXAGON_A2_combinew








#define Q6_R_max_RR __builtin_HEXAGON_A2_max








#define Q6_P_max_PP __builtin_HEXAGON_A2_maxp








#define Q6_R_maxu_RR __builtin_HEXAGON_A2_maxu








#define Q6_P_maxu_PP __builtin_HEXAGON_A2_maxup








#define Q6_R_min_RR __builtin_HEXAGON_A2_min








#define Q6_P_min_PP __builtin_HEXAGON_A2_minp








#define Q6_R_minu_RR __builtin_HEXAGON_A2_minu








#define Q6_P_minu_PP __builtin_HEXAGON_A2_minup








#define Q6_R_neg_R __builtin_HEXAGON_A2_neg








#define Q6_P_neg_P __builtin_HEXAGON_A2_negp








#define Q6_R_neg_R_sat __builtin_HEXAGON_A2_negsat








#define Q6_R_not_R __builtin_HEXAGON_A2_not








#define Q6_P_not_P __builtin_HEXAGON_A2_notp








#define Q6_R_or_RR __builtin_HEXAGON_A2_or








#define Q6_R_or_RI __builtin_HEXAGON_A2_orir








#define Q6_P_or_PP __builtin_HEXAGON_A2_orp








#define Q6_R_round_P_sat __builtin_HEXAGON_A2_roundsat








#define Q6_R_sat_P __builtin_HEXAGON_A2_sat








#define Q6_R_satb_R __builtin_HEXAGON_A2_satb








#define Q6_R_sath_R __builtin_HEXAGON_A2_sath








#define Q6_R_satub_R __builtin_HEXAGON_A2_satub








#define Q6_R_satuh_R __builtin_HEXAGON_A2_satuh








#define Q6_R_sub_RR __builtin_HEXAGON_A2_sub








#define Q6_R_sub_RhRh_s16 __builtin_HEXAGON_A2_subh_h16_hh








#define Q6_R_sub_RhRl_s16 __builtin_HEXAGON_A2_subh_h16_hl








#define Q6_R_sub_RlRh_s16 __builtin_HEXAGON_A2_subh_h16_lh








#define Q6_R_sub_RlRl_s16 __builtin_HEXAGON_A2_subh_h16_ll








#define Q6_R_sub_RhRh_sat_s16 __builtin_HEXAGON_A2_subh_h16_sat_hh








#define Q6_R_sub_RhRl_sat_s16 __builtin_HEXAGON_A2_subh_h16_sat_hl








#define Q6_R_sub_RlRh_sat_s16 __builtin_HEXAGON_A2_subh_h16_sat_lh








#define Q6_R_sub_RlRl_sat_s16 __builtin_HEXAGON_A2_subh_h16_sat_ll








#define Q6_R_sub_RlRh __builtin_HEXAGON_A2_subh_l16_hl








#define Q6_R_sub_RlRl __builtin_HEXAGON_A2_subh_l16_ll








#define Q6_R_sub_RlRh_sat __builtin_HEXAGON_A2_subh_l16_sat_hl








#define Q6_R_sub_RlRl_sat __builtin_HEXAGON_A2_subh_l16_sat_ll








#define Q6_P_sub_PP __builtin_HEXAGON_A2_subp








#define Q6_R_sub_IR __builtin_HEXAGON_A2_subri








#define Q6_R_sub_RR_sat __builtin_HEXAGON_A2_subsat








#define Q6_R_vaddh_RR __builtin_HEXAGON_A2_svaddh








#define Q6_R_vaddh_RR_sat __builtin_HEXAGON_A2_svaddhs








#define Q6_R_vadduh_RR_sat __builtin_HEXAGON_A2_svadduhs








#define Q6_R_vavgh_RR __builtin_HEXAGON_A2_svavgh








#define Q6_R_vavgh_RR_rnd __builtin_HEXAGON_A2_svavghs








#define Q6_R_vnavgh_RR __builtin_HEXAGON_A2_svnavgh








#define Q6_R_vsubh_RR __builtin_HEXAGON_A2_svsubh








#define Q6_R_vsubh_RR_sat __builtin_HEXAGON_A2_svsubhs








#define Q6_R_vsubuh_RR_sat __builtin_HEXAGON_A2_svsubuhs








#define Q6_R_swiz_R __builtin_HEXAGON_A2_swiz








#define Q6_R_sxtb_R __builtin_HEXAGON_A2_sxtb








#define Q6_R_sxth_R __builtin_HEXAGON_A2_sxth








#define Q6_P_sxtw_R __builtin_HEXAGON_A2_sxtw








#define Q6_R_equals_R __builtin_HEXAGON_A2_tfr








#define Q6_Rh_equals_I __builtin_HEXAGON_A2_tfrih








#define Q6_Rl_equals_I __builtin_HEXAGON_A2_tfril








#define Q6_P_equals_P __builtin_HEXAGON_A2_tfrp








#define Q6_P_equals_I __builtin_HEXAGON_A2_tfrpi








#define Q6_R_equals_I __builtin_HEXAGON_A2_tfrsi








#define Q6_P_vabsh_P __builtin_HEXAGON_A2_vabsh








#define Q6_P_vabsh_P_sat __builtin_HEXAGON_A2_vabshsat








#define Q6_P_vabsw_P __builtin_HEXAGON_A2_vabsw








#define Q6_P_vabsw_P_sat __builtin_HEXAGON_A2_vabswsat








#define Q6_P_vaddb_PP __builtin_HEXAGON_A2_vaddb_map








#define Q6_P_vaddh_PP __builtin_HEXAGON_A2_vaddh








#define Q6_P_vaddh_PP_sat __builtin_HEXAGON_A2_vaddhs








#define Q6_P_vaddub_PP __builtin_HEXAGON_A2_vaddub








#define Q6_P_vaddub_PP_sat __builtin_HEXAGON_A2_vaddubs








#define Q6_P_vadduh_PP_sat __builtin_HEXAGON_A2_vadduhs








#define Q6_P_vaddw_PP __builtin_HEXAGON_A2_vaddw








#define Q6_P_vaddw_PP_sat __builtin_HEXAGON_A2_vaddws








#define Q6_P_vavgh_PP __builtin_HEXAGON_A2_vavgh








#define Q6_P_vavgh_PP_crnd __builtin_HEXAGON_A2_vavghcr








#define Q6_P_vavgh_PP_rnd __builtin_HEXAGON_A2_vavghr








#define Q6_P_vavgub_PP __builtin_HEXAGON_A2_vavgub








#define Q6_P_vavgub_PP_rnd __builtin_HEXAGON_A2_vavgubr








#define Q6_P_vavguh_PP __builtin_HEXAGON_A2_vavguh








#define Q6_P_vavguh_PP_rnd __builtin_HEXAGON_A2_vavguhr








#define Q6_P_vavguw_PP __builtin_HEXAGON_A2_vavguw








#define Q6_P_vavguw_PP_rnd __builtin_HEXAGON_A2_vavguwr








#define Q6_P_vavgw_PP __builtin_HEXAGON_A2_vavgw








#define Q6_P_vavgw_PP_crnd __builtin_HEXAGON_A2_vavgwcr








#define Q6_P_vavgw_PP_rnd __builtin_HEXAGON_A2_vavgwr








#define Q6_p_vcmpb_eq_PP __builtin_HEXAGON_A2_vcmpbeq








#define Q6_p_vcmpb_gtu_PP __builtin_HEXAGON_A2_vcmpbgtu








#define Q6_p_vcmph_eq_PP __builtin_HEXAGON_A2_vcmpheq








#define Q6_p_vcmph_gt_PP __builtin_HEXAGON_A2_vcmphgt








#define Q6_p_vcmph_gtu_PP __builtin_HEXAGON_A2_vcmphgtu








#define Q6_p_vcmpw_eq_PP __builtin_HEXAGON_A2_vcmpweq








#define Q6_p_vcmpw_gt_PP __builtin_HEXAGON_A2_vcmpwgt








#define Q6_p_vcmpw_gtu_PP __builtin_HEXAGON_A2_vcmpwgtu








#define Q6_P_vconj_P_sat __builtin_HEXAGON_A2_vconj








#define Q6_P_vmaxb_PP __builtin_HEXAGON_A2_vmaxb








#define Q6_P_vmaxh_PP __builtin_HEXAGON_A2_vmaxh








#define Q6_P_vmaxub_PP __builtin_HEXAGON_A2_vmaxub








#define Q6_P_vmaxuh_PP __builtin_HEXAGON_A2_vmaxuh








#define Q6_P_vmaxuw_PP __builtin_HEXAGON_A2_vmaxuw








#define Q6_P_vmaxw_PP __builtin_HEXAGON_A2_vmaxw








#define Q6_P_vminb_PP __builtin_HEXAGON_A2_vminb








#define Q6_P_vminh_PP __builtin_HEXAGON_A2_vminh








#define Q6_P_vminub_PP __builtin_HEXAGON_A2_vminub








#define Q6_P_vminuh_PP __builtin_HEXAGON_A2_vminuh








#define Q6_P_vminuw_PP __builtin_HEXAGON_A2_vminuw








#define Q6_P_vminw_PP __builtin_HEXAGON_A2_vminw








#define Q6_P_vnavgh_PP __builtin_HEXAGON_A2_vnavgh








#define Q6_P_vnavgh_PP_crnd_sat __builtin_HEXAGON_A2_vnavghcr








#define Q6_P_vnavgh_PP_rnd_sat __builtin_HEXAGON_A2_vnavghr








#define Q6_P_vnavgw_PP __builtin_HEXAGON_A2_vnavgw








#define Q6_P_vnavgw_PP_crnd_sat __builtin_HEXAGON_A2_vnavgwcr








#define Q6_P_vnavgw_PP_rnd_sat __builtin_HEXAGON_A2_vnavgwr








#define Q6_P_vraddub_PP __builtin_HEXAGON_A2_vraddub








#define Q6_P_vraddubacc_PP __builtin_HEXAGON_A2_vraddub_acc








#define Q6_P_vrsadub_PP __builtin_HEXAGON_A2_vrsadub








#define Q6_P_vrsadubacc_PP __builtin_HEXAGON_A2_vrsadub_acc








#define Q6_P_vsubb_PP __builtin_HEXAGON_A2_vsubb_map








#define Q6_P_vsubh_PP __builtin_HEXAGON_A2_vsubh








#define Q6_P_vsubh_PP_sat __builtin_HEXAGON_A2_vsubhs








#define Q6_P_vsubub_PP __builtin_HEXAGON_A2_vsubub








#define Q6_P_vsubub_PP_sat __builtin_HEXAGON_A2_vsububs








#define Q6_P_vsubuh_PP_sat __builtin_HEXAGON_A2_vsubuhs








#define Q6_P_vsubw_PP __builtin_HEXAGON_A2_vsubw








#define Q6_P_vsubw_PP_sat __builtin_HEXAGON_A2_vsubws








#define Q6_R_xor_RR __builtin_HEXAGON_A2_xor








#define Q6_P_xor_PP __builtin_HEXAGON_A2_xorp








#define Q6_R_zxtb_R __builtin_HEXAGON_A2_zxtb








#define Q6_R_zxth_R __builtin_HEXAGON_A2_zxth








#define Q6_R_and_RnR __builtin_HEXAGON_A4_andn








#define Q6_P_and_PnP __builtin_HEXAGON_A4_andnp








#define Q6_P_bitsplit_RR __builtin_HEXAGON_A4_bitsplit








#define Q6_P_bitsplit_RI __builtin_HEXAGON_A4_bitspliti








#define Q6_p_boundscheck_RP __builtin_HEXAGON_A4_boundscheck








#define Q6_p_cmpb_eq_RR __builtin_HEXAGON_A4_cmpbeq








#define Q6_p_cmpb_eq_RI __builtin_HEXAGON_A4_cmpbeqi








#define Q6_p_cmpb_gt_RR __builtin_HEXAGON_A4_cmpbgt








#define Q6_p_cmpb_gt_RI __builtin_HEXAGON_A4_cmpbgti








#define Q6_p_cmpb_gtu_RR __builtin_HEXAGON_A4_cmpbgtu








#define Q6_p_cmpb_gtu_RI __builtin_HEXAGON_A4_cmpbgtui








#define Q6_p_cmph_eq_RR __builtin_HEXAGON_A4_cmpheq








#define Q6_p_cmph_eq_RI __builtin_HEXAGON_A4_cmpheqi








#define Q6_p_cmph_gt_RR __builtin_HEXAGON_A4_cmphgt








#define Q6_p_cmph_gt_RI __builtin_HEXAGON_A4_cmphgti








#define Q6_p_cmph_gtu_RR __builtin_HEXAGON_A4_cmphgtu








#define Q6_p_cmph_gtu_RI __builtin_HEXAGON_A4_cmphgtui








#define Q6_P_combine_IR __builtin_HEXAGON_A4_combineir








#define Q6_P_combine_RI __builtin_HEXAGON_A4_combineri








#define Q6_R_cround_RI __builtin_HEXAGON_A4_cround_ri








#define Q6_R_cround_RR __builtin_HEXAGON_A4_cround_rr








#define Q6_R_modwrap_RR __builtin_HEXAGON_A4_modwrapu








#define Q6_R_or_RnR __builtin_HEXAGON_A4_orn








#define Q6_P_or_PnP __builtin_HEXAGON_A4_ornp








#define Q6_R_cmp_eq_RR __builtin_HEXAGON_A4_rcmpeq








#define Q6_R_cmp_eq_RI __builtin_HEXAGON_A4_rcmpeqi








#define Q6_R_not_cmp_eq_RR __builtin_HEXAGON_A4_rcmpneq








#define Q6_R_not_cmp_eq_RI __builtin_HEXAGON_A4_rcmpneqi








#define Q6_R_round_RI __builtin_HEXAGON_A4_round_ri








#define Q6_R_round_RI_sat __builtin_HEXAGON_A4_round_ri_sat








#define Q6_R_round_RR __builtin_HEXAGON_A4_round_rr








#define Q6_R_round_RR_sat __builtin_HEXAGON_A4_round_rr_sat








#define Q6_p_tlbmatch_PR __builtin_HEXAGON_A4_tlbmatch








#define Q6_p_any8_vcmpb_eq_PP __builtin_HEXAGON_A4_vcmpbeq_any








#define Q6_p_vcmpb_eq_PI __builtin_HEXAGON_A4_vcmpbeqi








#define Q6_p_vcmpb_gt_PP __builtin_HEXAGON_A4_vcmpbgt








#define Q6_p_vcmpb_gt_PI __builtin_HEXAGON_A4_vcmpbgti








#define Q6_p_vcmpb_gtu_PI __builtin_HEXAGON_A4_vcmpbgtui








#define Q6_p_vcmph_eq_PI __builtin_HEXAGON_A4_vcmpheqi








#define Q6_p_vcmph_gt_PI __builtin_HEXAGON_A4_vcmphgti








#define Q6_p_vcmph_gtu_PI __builtin_HEXAGON_A4_vcmphgtui








#define Q6_p_vcmpw_eq_PI __builtin_HEXAGON_A4_vcmpweqi








#define Q6_p_vcmpw_gt_PI __builtin_HEXAGON_A4_vcmpwgti








#define Q6_p_vcmpw_gtu_PI __builtin_HEXAGON_A4_vcmpwgtui








#define Q6_P_vrmaxh_PR __builtin_HEXAGON_A4_vrmaxh








#define Q6_P_vrmaxuh_PR __builtin_HEXAGON_A4_vrmaxuh








#define Q6_P_vrmaxuw_PR __builtin_HEXAGON_A4_vrmaxuw








#define Q6_P_vrmaxw_PR __builtin_HEXAGON_A4_vrmaxw








#define Q6_P_vrminh_PR __builtin_HEXAGON_A4_vrminh








#define Q6_P_vrminuh_PR __builtin_HEXAGON_A4_vrminuh








#define Q6_P_vrminuw_PR __builtin_HEXAGON_A4_vrminuw








#define Q6_P_vrminw_PR __builtin_HEXAGON_A4_vrminw








#define Q6_R_vaddhub_PP_sat __builtin_HEXAGON_A5_vaddhubs








#define Q6_p_all8_p __builtin_HEXAGON_C2_all8








#define Q6_p_and_pp __builtin_HEXAGON_C2_and








#define Q6_p_and_pnp __builtin_HEXAGON_C2_andn








#define Q6_p_any8_p __builtin_HEXAGON_C2_any8








#define Q6_p_bitsclr_RR __builtin_HEXAGON_C2_bitsclr








#define Q6_p_bitsclr_RI __builtin_HEXAGON_C2_bitsclri








#define Q6_p_bitsset_RR __builtin_HEXAGON_C2_bitsset








#define Q6_p_cmp_eq_RR __builtin_HEXAGON_C2_cmpeq








#define Q6_p_cmp_eq_RI __builtin_HEXAGON_C2_cmpeqi








#define Q6_p_cmp_eq_PP __builtin_HEXAGON_C2_cmpeqp








#define Q6_p_cmp_ge_RI __builtin_HEXAGON_C2_cmpgei








#define Q6_p_cmp_geu_RI __builtin_HEXAGON_C2_cmpgeui








#define Q6_p_cmp_gt_RR __builtin_HEXAGON_C2_cmpgt








#define Q6_p_cmp_gt_RI __builtin_HEXAGON_C2_cmpgti








#define Q6_p_cmp_gt_PP __builtin_HEXAGON_C2_cmpgtp








#define Q6_p_cmp_gtu_RR __builtin_HEXAGON_C2_cmpgtu








#define Q6_p_cmp_gtu_RI __builtin_HEXAGON_C2_cmpgtui








#define Q6_p_cmp_gtu_PP __builtin_HEXAGON_C2_cmpgtup








#define Q6_p_cmp_lt_RR __builtin_HEXAGON_C2_cmplt








#define Q6_p_cmp_ltu_RR __builtin_HEXAGON_C2_cmpltu








#define Q6_P_mask_p __builtin_HEXAGON_C2_mask








#define Q6_R_mux_pRR __builtin_HEXAGON_C2_mux








#define Q6_R_mux_pII __builtin_HEXAGON_C2_muxii








#define Q6_R_mux_pRI __builtin_HEXAGON_C2_muxir








#define Q6_R_mux_pIR __builtin_HEXAGON_C2_muxri








#define Q6_p_not_p __builtin_HEXAGON_C2_not








#define Q6_p_or_pp __builtin_HEXAGON_C2_or








#define Q6_p_or_pnp __builtin_HEXAGON_C2_orn








#define Q6_p_equals_p __builtin_HEXAGON_C2_pxfer_map








#define Q6_R_equals_p __builtin_HEXAGON_C2_tfrpr








#define Q6_p_equals_R __builtin_HEXAGON_C2_tfrrp








#define Q6_R_vitpack_pp __builtin_HEXAGON_C2_vitpack








#define Q6_P_vmux_pPP __builtin_HEXAGON_C2_vmux








#define Q6_p_xor_pp __builtin_HEXAGON_C2_xor








#define Q6_p_and_and_ppp __builtin_HEXAGON_C4_and_and








#define Q6_p_and_and_ppnp __builtin_HEXAGON_C4_and_andn








#define Q6_p_and_or_ppp __builtin_HEXAGON_C4_and_or








#define Q6_p_and_or_ppnp __builtin_HEXAGON_C4_and_orn








#define Q6_p_not_cmp_gt_RR __builtin_HEXAGON_C4_cmplte








#define Q6_p_not_cmp_gt_RI __builtin_HEXAGON_C4_cmpltei








#define Q6_p_not_cmp_gtu_RR __builtin_HEXAGON_C4_cmplteu








#define Q6_p_not_cmp_gtu_RI __builtin_HEXAGON_C4_cmplteui








#define Q6_p_not_cmp_eq_RR __builtin_HEXAGON_C4_cmpneq








#define Q6_p_not_cmp_eq_RI __builtin_HEXAGON_C4_cmpneqi








#define Q6_p_fastcorner9_pp __builtin_HEXAGON_C4_fastcorner9








#define Q6_p_not_fastcorner9_pp __builtin_HEXAGON_C4_fastcorner9_not








#define Q6_p_not_bitsclr_RR __builtin_HEXAGON_C4_nbitsclr








#define Q6_p_not_bitsclr_RI __builtin_HEXAGON_C4_nbitsclri








#define Q6_p_not_bitsset_RR __builtin_HEXAGON_C4_nbitsset








#define Q6_p_or_and_ppp __builtin_HEXAGON_C4_or_and








#define Q6_p_or_and_ppnp __builtin_HEXAGON_C4_or_andn








#define Q6_p_or_or_ppp __builtin_HEXAGON_C4_or_or








#define Q6_p_or_or_ppnp __builtin_HEXAGON_C4_or_orn








#define Q6_P_convert_d2df_P __builtin_HEXAGON_F2_conv_d2df








#define Q6_R_convert_d2sf_P __builtin_HEXAGON_F2_conv_d2sf








#define Q6_P_convert_df2d_P __builtin_HEXAGON_F2_conv_df2d








#define Q6_P_convert_df2d_P_chop __builtin_HEXAGON_F2_conv_df2d_chop








#define Q6_R_convert_df2sf_P __builtin_HEXAGON_F2_conv_df2sf








#define Q6_P_convert_df2ud_P __builtin_HEXAGON_F2_conv_df2ud








#define Q6_P_convert_df2ud_P_chop __builtin_HEXAGON_F2_conv_df2ud_chop








#define Q6_R_convert_df2uw_P __builtin_HEXAGON_F2_conv_df2uw








#define Q6_R_convert_df2uw_P_chop __builtin_HEXAGON_F2_conv_df2uw_chop








#define Q6_R_convert_df2w_P __builtin_HEXAGON_F2_conv_df2w








#define Q6_R_convert_df2w_P_chop __builtin_HEXAGON_F2_conv_df2w_chop








#define Q6_P_convert_sf2d_R __builtin_HEXAGON_F2_conv_sf2d








#define Q6_P_convert_sf2d_R_chop __builtin_HEXAGON_F2_conv_sf2d_chop








#define Q6_P_convert_sf2df_R __builtin_HEXAGON_F2_conv_sf2df








#define Q6_P_convert_sf2ud_R __builtin_HEXAGON_F2_conv_sf2ud








#define Q6_P_convert_sf2ud_R_chop __builtin_HEXAGON_F2_conv_sf2ud_chop








#define Q6_R_convert_sf2uw_R __builtin_HEXAGON_F2_conv_sf2uw








#define Q6_R_convert_sf2uw_R_chop __builtin_HEXAGON_F2_conv_sf2uw_chop








#define Q6_R_convert_sf2w_R __builtin_HEXAGON_F2_conv_sf2w








#define Q6_R_convert_sf2w_R_chop __builtin_HEXAGON_F2_conv_sf2w_chop








#define Q6_P_convert_ud2df_P __builtin_HEXAGON_F2_conv_ud2df








#define Q6_R_convert_ud2sf_P __builtin_HEXAGON_F2_conv_ud2sf








#define Q6_P_convert_uw2df_R __builtin_HEXAGON_F2_conv_uw2df








#define Q6_R_convert_uw2sf_R __builtin_HEXAGON_F2_conv_uw2sf








#define Q6_P_convert_w2df_R __builtin_HEXAGON_F2_conv_w2df








#define Q6_R_convert_w2sf_R __builtin_HEXAGON_F2_conv_w2sf








#define Q6_p_dfclass_PI __builtin_HEXAGON_F2_dfclass








#define Q6_p_dfcmp_eq_PP __builtin_HEXAGON_F2_dfcmpeq








#define Q6_p_dfcmp_ge_PP __builtin_HEXAGON_F2_dfcmpge








#define Q6_p_dfcmp_gt_PP __builtin_HEXAGON_F2_dfcmpgt








#define Q6_p_dfcmp_uo_PP __builtin_HEXAGON_F2_dfcmpuo








#define Q6_P_dfmake_I_neg __builtin_HEXAGON_F2_dfimm_n








#define Q6_P_dfmake_I_pos __builtin_HEXAGON_F2_dfimm_p








#define Q6_R_sfadd_RR __builtin_HEXAGON_F2_sfadd








#define Q6_p_sfclass_RI __builtin_HEXAGON_F2_sfclass








#define Q6_p_sfcmp_eq_RR __builtin_HEXAGON_F2_sfcmpeq








#define Q6_p_sfcmp_ge_RR __builtin_HEXAGON_F2_sfcmpge








#define Q6_p_sfcmp_gt_RR __builtin_HEXAGON_F2_sfcmpgt








#define Q6_p_sfcmp_uo_RR __builtin_HEXAGON_F2_sfcmpuo








#define Q6_R_sffixupd_RR __builtin_HEXAGON_F2_sffixupd








#define Q6_R_sffixupn_RR __builtin_HEXAGON_F2_sffixupn








#define Q6_R_sffixupr_R __builtin_HEXAGON_F2_sffixupr








#define Q6_R_sfmpyacc_RR __builtin_HEXAGON_F2_sffma








#define Q6_R_sfmpyacc_RR_lib __builtin_HEXAGON_F2_sffma_lib








#define Q6_R_sfmpyacc_RRp_scale __builtin_HEXAGON_F2_sffma_sc








#define Q6_R_sfmpynac_RR __builtin_HEXAGON_F2_sffms








#define Q6_R_sfmpynac_RR_lib __builtin_HEXAGON_F2_sffms_lib








#define Q6_R_sfmake_I_neg __builtin_HEXAGON_F2_sfimm_n








#define Q6_R_sfmake_I_pos __builtin_HEXAGON_F2_sfimm_p








#define Q6_R_sfmax_RR __builtin_HEXAGON_F2_sfmax








#define Q6_R_sfmin_RR __builtin_HEXAGON_F2_sfmin








#define Q6_R_sfmpy_RR __builtin_HEXAGON_F2_sfmpy








#define Q6_R_sfsub_RR __builtin_HEXAGON_F2_sfsub








#define Q6_R_memb_IM_circ __builtin_HEXAGON_L2_loadrb_pci








#define Q6_R_memb_M_circ __builtin_HEXAGON_L2_loadrb_pcr








#define Q6_P_memd_IM_circ __builtin_HEXAGON_L2_loadrd_pci








#define Q6_P_memd_M_circ __builtin_HEXAGON_L2_loadrd_pcr








#define Q6_R_memh_IM_circ __builtin_HEXAGON_L2_loadrh_pci








#define Q6_R_memh_M_circ __builtin_HEXAGON_L2_loadrh_pcr








#define Q6_R_memw_IM_circ __builtin_HEXAGON_L2_loadri_pci








#define Q6_R_memw_M_circ __builtin_HEXAGON_L2_loadri_pcr








#define Q6_R_memub_IM_circ __builtin_HEXAGON_L2_loadrub_pci








#define Q6_R_memub_M_circ __builtin_HEXAGON_L2_loadrub_pcr








#define Q6_R_memuh_IM_circ __builtin_HEXAGON_L2_loadruh_pci








#define Q6_R_memuh_M_circ __builtin_HEXAGON_L2_loadruh_pcr








#define Q6_R_addacc_RR __builtin_HEXAGON_M2_acci








#define Q6_R_addacc_RI __builtin_HEXAGON_M2_accii








#define Q6_P_cmpyiacc_RR __builtin_HEXAGON_M2_cmaci_s0








#define Q6_P_cmpyracc_RR __builtin_HEXAGON_M2_cmacr_s0








#define Q6_P_cmpyacc_RR_sat __builtin_HEXAGON_M2_cmacs_s0








#define Q6_P_cmpyacc_RR_s1_sat __builtin_HEXAGON_M2_cmacs_s1








#define Q6_P_cmpyacc_RR_conj_sat __builtin_HEXAGON_M2_cmacsc_s0








#define Q6_P_cmpyacc_RR_conj_s1_sat __builtin_HEXAGON_M2_cmacsc_s1








#define Q6_P_cmpyi_RR __builtin_HEXAGON_M2_cmpyi_s0








#define Q6_P_cmpyr_RR __builtin_HEXAGON_M2_cmpyr_s0








#define Q6_R_cmpy_RR_rnd_sat __builtin_HEXAGON_M2_cmpyrs_s0








#define Q6_R_cmpy_RR_s1_rnd_sat __builtin_HEXAGON_M2_cmpyrs_s1








#define Q6_R_cmpy_RR_conj_rnd_sat __builtin_HEXAGON_M2_cmpyrsc_s0








#define Q6_R_cmpy_RR_conj_s1_rnd_sat __builtin_HEXAGON_M2_cmpyrsc_s1








#define Q6_P_cmpy_RR_sat __builtin_HEXAGON_M2_cmpys_s0








#define Q6_P_cmpy_RR_s1_sat __builtin_HEXAGON_M2_cmpys_s1








#define Q6_P_cmpy_RR_conj_sat __builtin_HEXAGON_M2_cmpysc_s0








#define Q6_P_cmpy_RR_conj_s1_sat __builtin_HEXAGON_M2_cmpysc_s1








#define Q6_P_cmpynac_RR_sat __builtin_HEXAGON_M2_cnacs_s0








#define Q6_P_cmpynac_RR_s1_sat __builtin_HEXAGON_M2_cnacs_s1








#define Q6_P_cmpynac_RR_conj_sat __builtin_HEXAGON_M2_cnacsc_s0








#define Q6_P_cmpynac_RR_conj_s1_sat __builtin_HEXAGON_M2_cnacsc_s1








#define Q6_P_mpyacc_RR __builtin_HEXAGON_M2_dpmpyss_acc_s0








#define Q6_P_mpynac_RR __builtin_HEXAGON_M2_dpmpyss_nac_s0








#define Q6_R_mpy_RR_rnd __builtin_HEXAGON_M2_dpmpyss_rnd_s0








#define Q6_P_mpy_RR __builtin_HEXAGON_M2_dpmpyss_s0








#define Q6_P_mpyuacc_RR __builtin_HEXAGON_M2_dpmpyuu_acc_s0








#define Q6_P_mpyunac_RR __builtin_HEXAGON_M2_dpmpyuu_nac_s0








#define Q6_P_mpyu_RR __builtin_HEXAGON_M2_dpmpyuu_s0








#define Q6_R_mpy_RRh_s1_rnd_sat __builtin_HEXAGON_M2_hmmpyh_rs1








#define Q6_R_mpy_RRh_s1_sat __builtin_HEXAGON_M2_hmmpyh_s1








#define Q6_R_mpy_RRl_s1_rnd_sat __builtin_HEXAGON_M2_hmmpyl_rs1








#define Q6_R_mpy_RRl_s1_sat __builtin_HEXAGON_M2_hmmpyl_s1








#define Q6_R_mpyiacc_RR __builtin_HEXAGON_M2_maci








#define Q6_R_mpyinac_RI __builtin_HEXAGON_M2_macsin








#define Q6_R_mpyiacc_RI __builtin_HEXAGON_M2_macsip








#define Q6_P_vmpywohacc_PP_rnd_sat __builtin_HEXAGON_M2_mmachs_rs0








#define Q6_P_vmpywohacc_PP_s1_rnd_sat __builtin_HEXAGON_M2_mmachs_rs1








#define Q6_P_vmpywohacc_PP_sat __builtin_HEXAGON_M2_mmachs_s0








#define Q6_P_vmpywohacc_PP_s1_sat __builtin_HEXAGON_M2_mmachs_s1








#define Q6_P_vmpywehacc_PP_rnd_sat __builtin_HEXAGON_M2_mmacls_rs0








#define Q6_P_vmpywehacc_PP_s1_rnd_sat __builtin_HEXAGON_M2_mmacls_rs1








#define Q6_P_vmpywehacc_PP_sat __builtin_HEXAGON_M2_mmacls_s0








#define Q6_P_vmpywehacc_PP_s1_sat __builtin_HEXAGON_M2_mmacls_s1








#define Q6_P_vmpywouhacc_PP_rnd_sat __builtin_HEXAGON_M2_mmacuhs_rs0








#define Q6_P_vmpywouhacc_PP_s1_rnd_sat __builtin_HEXAGON_M2_mmacuhs_rs1








#define Q6_P_vmpywouhacc_PP_sat __builtin_HEXAGON_M2_mmacuhs_s0








#define Q6_P_vmpywouhacc_PP_s1_sat __builtin_HEXAGON_M2_mmacuhs_s1








#define Q6_P_vmpyweuhacc_PP_rnd_sat __builtin_HEXAGON_M2_mmaculs_rs0








#define Q6_P_vmpyweuhacc_PP_s1_rnd_sat __builtin_HEXAGON_M2_mmaculs_rs1








#define Q6_P_vmpyweuhacc_PP_sat __builtin_HEXAGON_M2_mmaculs_s0








#define Q6_P_vmpyweuhacc_PP_s1_sat __builtin_HEXAGON_M2_mmaculs_s1








#define Q6_P_vmpywoh_PP_rnd_sat __builtin_HEXAGON_M2_mmpyh_rs0








#define Q6_P_vmpywoh_PP_s1_rnd_sat __builtin_HEXAGON_M2_mmpyh_rs1








#define Q6_P_vmpywoh_PP_sat __builtin_HEXAGON_M2_mmpyh_s0








#define Q6_P_vmpywoh_PP_s1_sat __builtin_HEXAGON_M2_mmpyh_s1








#define Q6_P_vmpyweh_PP_rnd_sat __builtin_HEXAGON_M2_mmpyl_rs0








#define Q6_P_vmpyweh_PP_s1_rnd_sat __builtin_HEXAGON_M2_mmpyl_rs1








#define Q6_P_vmpyweh_PP_sat __builtin_HEXAGON_M2_mmpyl_s0








#define Q6_P_vmpyweh_PP_s1_sat __builtin_HEXAGON_M2_mmpyl_s1








#define Q6_P_vmpywouh_PP_rnd_sat __builtin_HEXAGON_M2_mmpyuh_rs0








#define Q6_P_vmpywouh_PP_s1_rnd_sat __builtin_HEXAGON_M2_mmpyuh_rs1








#define Q6_P_vmpywouh_PP_sat __builtin_HEXAGON_M2_mmpyuh_s0








#define Q6_P_vmpywouh_PP_s1_sat __builtin_HEXAGON_M2_mmpyuh_s1








#define Q6_P_vmpyweuh_PP_rnd_sat __builtin_HEXAGON_M2_mmpyul_rs0








#define Q6_P_vmpyweuh_PP_s1_rnd_sat __builtin_HEXAGON_M2_mmpyul_rs1








#define Q6_P_vmpyweuh_PP_sat __builtin_HEXAGON_M2_mmpyul_s0








#define Q6_P_vmpyweuh_PP_s1_sat __builtin_HEXAGON_M2_mmpyul_s1








#define Q6_R_mpyacc_RhRh __builtin_HEXAGON_M2_mpy_acc_hh_s0








#define Q6_R_mpyacc_RhRh_s1 __builtin_HEXAGON_M2_mpy_acc_hh_s1








#define Q6_R_mpyacc_RhRl __builtin_HEXAGON_M2_mpy_acc_hl_s0








#define Q6_R_mpyacc_RhRl_s1 __builtin_HEXAGON_M2_mpy_acc_hl_s1








#define Q6_R_mpyacc_RlRh __builtin_HEXAGON_M2_mpy_acc_lh_s0








#define Q6_R_mpyacc_RlRh_s1 __builtin_HEXAGON_M2_mpy_acc_lh_s1








#define Q6_R_mpyacc_RlRl __builtin_HEXAGON_M2_mpy_acc_ll_s0








#define Q6_R_mpyacc_RlRl_s1 __builtin_HEXAGON_M2_mpy_acc_ll_s1








#define Q6_R_mpyacc_RhRh_sat __builtin_HEXAGON_M2_mpy_acc_sat_hh_s0








#define Q6_R_mpyacc_RhRh_s1_sat __builtin_HEXAGON_M2_mpy_acc_sat_hh_s1








#define Q6_R_mpyacc_RhRl_sat __builtin_HEXAGON_M2_mpy_acc_sat_hl_s0








#define Q6_R_mpyacc_RhRl_s1_sat __builtin_HEXAGON_M2_mpy_acc_sat_hl_s1








#define Q6_R_mpyacc_RlRh_sat __builtin_HEXAGON_M2_mpy_acc_sat_lh_s0








#define Q6_R_mpyacc_RlRh_s1_sat __builtin_HEXAGON_M2_mpy_acc_sat_lh_s1








#define Q6_R_mpyacc_RlRl_sat __builtin_HEXAGON_M2_mpy_acc_sat_ll_s0








#define Q6_R_mpyacc_RlRl_s1_sat __builtin_HEXAGON_M2_mpy_acc_sat_ll_s1








#define Q6_R_mpy_RhRh __builtin_HEXAGON_M2_mpy_hh_s0








#define Q6_R_mpy_RhRh_s1 __builtin_HEXAGON_M2_mpy_hh_s1








#define Q6_R_mpy_RhRl __builtin_HEXAGON_M2_mpy_hl_s0








#define Q6_R_mpy_RhRl_s1 __builtin_HEXAGON_M2_mpy_hl_s1








#define Q6_R_mpy_RlRh __builtin_HEXAGON_M2_mpy_lh_s0








#define Q6_R_mpy_RlRh_s1 __builtin_HEXAGON_M2_mpy_lh_s1








#define Q6_R_mpy_RlRl __builtin_HEXAGON_M2_mpy_ll_s0








#define Q6_R_mpy_RlRl_s1 __builtin_HEXAGON_M2_mpy_ll_s1








#define Q6_R_mpynac_RhRh __builtin_HEXAGON_M2_mpy_nac_hh_s0








#define Q6_R_mpynac_RhRh_s1 __builtin_HEXAGON_M2_mpy_nac_hh_s1








#define Q6_R_mpynac_RhRl __builtin_HEXAGON_M2_mpy_nac_hl_s0








#define Q6_R_mpynac_RhRl_s1 __builtin_HEXAGON_M2_mpy_nac_hl_s1








#define Q6_R_mpynac_RlRh __builtin_HEXAGON_M2_mpy_nac_lh_s0








#define Q6_R_mpynac_RlRh_s1 __builtin_HEXAGON_M2_mpy_nac_lh_s1








#define Q6_R_mpynac_RlRl __builtin_HEXAGON_M2_mpy_nac_ll_s0








#define Q6_R_mpynac_RlRl_s1 __builtin_HEXAGON_M2_mpy_nac_ll_s1








#define Q6_R_mpynac_RhRh_sat __builtin_HEXAGON_M2_mpy_nac_sat_hh_s0








#define Q6_R_mpynac_RhRh_s1_sat __builtin_HEXAGON_M2_mpy_nac_sat_hh_s1








#define Q6_R_mpynac_RhRl_sat __builtin_HEXAGON_M2_mpy_nac_sat_hl_s0








#define Q6_R_mpynac_RhRl_s1_sat __builtin_HEXAGON_M2_mpy_nac_sat_hl_s1








#define Q6_R_mpynac_RlRh_sat __builtin_HEXAGON_M2_mpy_nac_sat_lh_s0








#define Q6_R_mpynac_RlRh_s1_sat __builtin_HEXAGON_M2_mpy_nac_sat_lh_s1








#define Q6_R_mpynac_RlRl_sat __builtin_HEXAGON_M2_mpy_nac_sat_ll_s0








#define Q6_R_mpynac_RlRl_s1_sat __builtin_HEXAGON_M2_mpy_nac_sat_ll_s1








#define Q6_R_mpy_RhRh_rnd __builtin_HEXAGON_M2_mpy_rnd_hh_s0








#define Q6_R_mpy_RhRh_s1_rnd __builtin_HEXAGON_M2_mpy_rnd_hh_s1








#define Q6_R_mpy_RhRl_rnd __builtin_HEXAGON_M2_mpy_rnd_hl_s0








#define Q6_R_mpy_RhRl_s1_rnd __builtin_HEXAGON_M2_mpy_rnd_hl_s1








#define Q6_R_mpy_RlRh_rnd __builtin_HEXAGON_M2_mpy_rnd_lh_s0








#define Q6_R_mpy_RlRh_s1_rnd __builtin_HEXAGON_M2_mpy_rnd_lh_s1








#define Q6_R_mpy_RlRl_rnd __builtin_HEXAGON_M2_mpy_rnd_ll_s0








#define Q6_R_mpy_RlRl_s1_rnd __builtin_HEXAGON_M2_mpy_rnd_ll_s1








#define Q6_R_mpy_RhRh_sat __builtin_HEXAGON_M2_mpy_sat_hh_s0








#define Q6_R_mpy_RhRh_s1_sat __builtin_HEXAGON_M2_mpy_sat_hh_s1








#define Q6_R_mpy_RhRl_sat __builtin_HEXAGON_M2_mpy_sat_hl_s0








#define Q6_R_mpy_RhRl_s1_sat __builtin_HEXAGON_M2_mpy_sat_hl_s1








#define Q6_R_mpy_RlRh_sat __builtin_HEXAGON_M2_mpy_sat_lh_s0








#define Q6_R_mpy_RlRh_s1_sat __builtin_HEXAGON_M2_mpy_sat_lh_s1








#define Q6_R_mpy_RlRl_sat __builtin_HEXAGON_M2_mpy_sat_ll_s0








#define Q6_R_mpy_RlRl_s1_sat __builtin_HEXAGON_M2_mpy_sat_ll_s1








#define Q6_R_mpy_RhRh_rnd_sat __builtin_HEXAGON_M2_mpy_sat_rnd_hh_s0








#define Q6_R_mpy_RhRh_s1_rnd_sat __builtin_HEXAGON_M2_mpy_sat_rnd_hh_s1








#define Q6_R_mpy_RhRl_rnd_sat __builtin_HEXAGON_M2_mpy_sat_rnd_hl_s0








#define Q6_R_mpy_RhRl_s1_rnd_sat __builtin_HEXAGON_M2_mpy_sat_rnd_hl_s1








#define Q6_R_mpy_RlRh_rnd_sat __builtin_HEXAGON_M2_mpy_sat_rnd_lh_s0








#define Q6_R_mpy_RlRh_s1_rnd_sat __builtin_HEXAGON_M2_mpy_sat_rnd_lh_s1








#define Q6_R_mpy_RlRl_rnd_sat __builtin_HEXAGON_M2_mpy_sat_rnd_ll_s0








#define Q6_R_mpy_RlRl_s1_rnd_sat __builtin_HEXAGON_M2_mpy_sat_rnd_ll_s1








#define Q6_R_mpy_RR __builtin_HEXAGON_M2_mpy_up








#define Q6_R_mpy_RR_s1 __builtin_HEXAGON_M2_mpy_up_s1








#define Q6_R_mpy_RR_s1_sat __builtin_HEXAGON_M2_mpy_up_s1_sat








#define Q6_P_mpyacc_RhRh __builtin_HEXAGON_M2_mpyd_acc_hh_s0








#define Q6_P_mpyacc_RhRh_s1 __builtin_HEXAGON_M2_mpyd_acc_hh_s1








#define Q6_P_mpyacc_RhRl __builtin_HEXAGON_M2_mpyd_acc_hl_s0








#define Q6_P_mpyacc_RhRl_s1 __builtin_HEXAGON_M2_mpyd_acc_hl_s1








#define Q6_P_mpyacc_RlRh __builtin_HEXAGON_M2_mpyd_acc_lh_s0








#define Q6_P_mpyacc_RlRh_s1 __builtin_HEXAGON_M2_mpyd_acc_lh_s1








#define Q6_P_mpyacc_RlRl __builtin_HEXAGON_M2_mpyd_acc_ll_s0








#define Q6_P_mpyacc_RlRl_s1 __builtin_HEXAGON_M2_mpyd_acc_ll_s1








#define Q6_P_mpy_RhRh __builtin_HEXAGON_M2_mpyd_hh_s0








#define Q6_P_mpy_RhRh_s1 __builtin_HEXAGON_M2_mpyd_hh_s1








#define Q6_P_mpy_RhRl __builtin_HEXAGON_M2_mpyd_hl_s0








#define Q6_P_mpy_RhRl_s1 __builtin_HEXAGON_M2_mpyd_hl_s1








#define Q6_P_mpy_RlRh __builtin_HEXAGON_M2_mpyd_lh_s0








#define Q6_P_mpy_RlRh_s1 __builtin_HEXAGON_M2_mpyd_lh_s1








#define Q6_P_mpy_RlRl __builtin_HEXAGON_M2_mpyd_ll_s0








#define Q6_P_mpy_RlRl_s1 __builtin_HEXAGON_M2_mpyd_ll_s1








#define Q6_P_mpynac_RhRh __builtin_HEXAGON_M2_mpyd_nac_hh_s0








#define Q6_P_mpynac_RhRh_s1 __builtin_HEXAGON_M2_mpyd_nac_hh_s1








#define Q6_P_mpynac_RhRl __builtin_HEXAGON_M2_mpyd_nac_hl_s0








#define Q6_P_mpynac_RhRl_s1 __builtin_HEXAGON_M2_mpyd_nac_hl_s1








#define Q6_P_mpynac_RlRh __builtin_HEXAGON_M2_mpyd_nac_lh_s0








#define Q6_P_mpynac_RlRh_s1 __builtin_HEXAGON_M2_mpyd_nac_lh_s1








#define Q6_P_mpynac_RlRl __builtin_HEXAGON_M2_mpyd_nac_ll_s0








#define Q6_P_mpynac_RlRl_s1 __builtin_HEXAGON_M2_mpyd_nac_ll_s1








#define Q6_P_mpy_RhRh_rnd __builtin_HEXAGON_M2_mpyd_rnd_hh_s0








#define Q6_P_mpy_RhRh_s1_rnd __builtin_HEXAGON_M2_mpyd_rnd_hh_s1








#define Q6_P_mpy_RhRl_rnd __builtin_HEXAGON_M2_mpyd_rnd_hl_s0








#define Q6_P_mpy_RhRl_s1_rnd __builtin_HEXAGON_M2_mpyd_rnd_hl_s1








#define Q6_P_mpy_RlRh_rnd __builtin_HEXAGON_M2_mpyd_rnd_lh_s0








#define Q6_P_mpy_RlRh_s1_rnd __builtin_HEXAGON_M2_mpyd_rnd_lh_s1








#define Q6_P_mpy_RlRl_rnd __builtin_HEXAGON_M2_mpyd_rnd_ll_s0








#define Q6_P_mpy_RlRl_s1_rnd __builtin_HEXAGON_M2_mpyd_rnd_ll_s1








#define Q6_R_mpyi_RR __builtin_HEXAGON_M2_mpyi








#define Q6_R_mpyi_RI __builtin_HEXAGON_M2_mpysmi








#define Q6_R_mpysu_RR __builtin_HEXAGON_M2_mpysu_up








#define Q6_R_mpyuacc_RhRh __builtin_HEXAGON_M2_mpyu_acc_hh_s0








#define Q6_R_mpyuacc_RhRh_s1 __builtin_HEXAGON_M2_mpyu_acc_hh_s1








#define Q6_R_mpyuacc_RhRl __builtin_HEXAGON_M2_mpyu_acc_hl_s0








#define Q6_R_mpyuacc_RhRl_s1 __builtin_HEXAGON_M2_mpyu_acc_hl_s1








#define Q6_R_mpyuacc_RlRh __builtin_HEXAGON_M2_mpyu_acc_lh_s0








#define Q6_R_mpyuacc_RlRh_s1 __builtin_HEXAGON_M2_mpyu_acc_lh_s1








#define Q6_R_mpyuacc_RlRl __builtin_HEXAGON_M2_mpyu_acc_ll_s0








#define Q6_R_mpyuacc_RlRl_s1 __builtin_HEXAGON_M2_mpyu_acc_ll_s1








#define Q6_R_mpyu_RhRh __builtin_HEXAGON_M2_mpyu_hh_s0








#define Q6_R_mpyu_RhRh_s1 __builtin_HEXAGON_M2_mpyu_hh_s1








#define Q6_R_mpyu_RhRl __builtin_HEXAGON_M2_mpyu_hl_s0








#define Q6_R_mpyu_RhRl_s1 __builtin_HEXAGON_M2_mpyu_hl_s1








#define Q6_R_mpyu_RlRh __builtin_HEXAGON_M2_mpyu_lh_s0








#define Q6_R_mpyu_RlRh_s1 __builtin_HEXAGON_M2_mpyu_lh_s1








#define Q6_R_mpyu_RlRl __builtin_HEXAGON_M2_mpyu_ll_s0








#define Q6_R_mpyu_RlRl_s1 __builtin_HEXAGON_M2_mpyu_ll_s1








#define Q6_R_mpyunac_RhRh __builtin_HEXAGON_M2_mpyu_nac_hh_s0








#define Q6_R_mpyunac_RhRh_s1 __builtin_HEXAGON_M2_mpyu_nac_hh_s1








#define Q6_R_mpyunac_RhRl __builtin_HEXAGON_M2_mpyu_nac_hl_s0








#define Q6_R_mpyunac_RhRl_s1 __builtin_HEXAGON_M2_mpyu_nac_hl_s1








#define Q6_R_mpyunac_RlRh __builtin_HEXAGON_M2_mpyu_nac_lh_s0








#define Q6_R_mpyunac_RlRh_s1 __builtin_HEXAGON_M2_mpyu_nac_lh_s1








#define Q6_R_mpyunac_RlRl __builtin_HEXAGON_M2_mpyu_nac_ll_s0








#define Q6_R_mpyunac_RlRl_s1 __builtin_HEXAGON_M2_mpyu_nac_ll_s1








#define Q6_R_mpyu_RR __builtin_HEXAGON_M2_mpyu_up








#define Q6_P_mpyuacc_RhRh __builtin_HEXAGON_M2_mpyud_acc_hh_s0








#define Q6_P_mpyuacc_RhRh_s1 __builtin_HEXAGON_M2_mpyud_acc_hh_s1








#define Q6_P_mpyuacc_RhRl __builtin_HEXAGON_M2_mpyud_acc_hl_s0








#define Q6_P_mpyuacc_RhRl_s1 __builtin_HEXAGON_M2_mpyud_acc_hl_s1








#define Q6_P_mpyuacc_RlRh __builtin_HEXAGON_M2_mpyud_acc_lh_s0








#define Q6_P_mpyuacc_RlRh_s1 __builtin_HEXAGON_M2_mpyud_acc_lh_s1








#define Q6_P_mpyuacc_RlRl __builtin_HEXAGON_M2_mpyud_acc_ll_s0








#define Q6_P_mpyuacc_RlRl_s1 __builtin_HEXAGON_M2_mpyud_acc_ll_s1








#define Q6_P_mpyu_RhRh __builtin_HEXAGON_M2_mpyud_hh_s0








#define Q6_P_mpyu_RhRh_s1 __builtin_HEXAGON_M2_mpyud_hh_s1








#define Q6_P_mpyu_RhRl __builtin_HEXAGON_M2_mpyud_hl_s0








#define Q6_P_mpyu_RhRl_s1 __builtin_HEXAGON_M2_mpyud_hl_s1








#define Q6_P_mpyu_RlRh __builtin_HEXAGON_M2_mpyud_lh_s0








#define Q6_P_mpyu_RlRh_s1 __builtin_HEXAGON_M2_mpyud_lh_s1








#define Q6_P_mpyu_RlRl __builtin_HEXAGON_M2_mpyud_ll_s0








#define Q6_P_mpyu_RlRl_s1 __builtin_HEXAGON_M2_mpyud_ll_s1








#define Q6_P_mpyunac_RhRh __builtin_HEXAGON_M2_mpyud_nac_hh_s0








#define Q6_P_mpyunac_RhRh_s1 __builtin_HEXAGON_M2_mpyud_nac_hh_s1








#define Q6_P_mpyunac_RhRl __builtin_HEXAGON_M2_mpyud_nac_hl_s0








#define Q6_P_mpyunac_RhRl_s1 __builtin_HEXAGON_M2_mpyud_nac_hl_s1








#define Q6_P_mpyunac_RlRh __builtin_HEXAGON_M2_mpyud_nac_lh_s0








#define Q6_P_mpyunac_RlRh_s1 __builtin_HEXAGON_M2_mpyud_nac_lh_s1








#define Q6_P_mpyunac_RlRl __builtin_HEXAGON_M2_mpyud_nac_ll_s0








#define Q6_P_mpyunac_RlRl_s1 __builtin_HEXAGON_M2_mpyud_nac_ll_s1








#define Q6_R_mpyui_RR __builtin_HEXAGON_M2_mpyui








#define Q6_R_addnac_RR __builtin_HEXAGON_M2_nacci








#define Q6_R_addnac_RI __builtin_HEXAGON_M2_naccii








#define Q6_R_subacc_RR __builtin_HEXAGON_M2_subacc








#define Q6_P_vabsdiffh_PP __builtin_HEXAGON_M2_vabsdiffh








#define Q6_P_vabsdiffw_PP __builtin_HEXAGON_M2_vabsdiffw








#define Q6_P_vcmpyiacc_PP_sat __builtin_HEXAGON_M2_vcmac_s0_sat_i








#define Q6_P_vcmpyracc_PP_sat __builtin_HEXAGON_M2_vcmac_s0_sat_r








#define Q6_P_vcmpyi_PP_sat __builtin_HEXAGON_M2_vcmpy_s0_sat_i








#define Q6_P_vcmpyr_PP_sat __builtin_HEXAGON_M2_vcmpy_s0_sat_r








#define Q6_P_vcmpyi_PP_s1_sat __builtin_HEXAGON_M2_vcmpy_s1_sat_i








#define Q6_P_vcmpyr_PP_s1_sat __builtin_HEXAGON_M2_vcmpy_s1_sat_r








#define Q6_P_vdmpyacc_PP_sat __builtin_HEXAGON_M2_vdmacs_s0








#define Q6_P_vdmpyacc_PP_s1_sat __builtin_HEXAGON_M2_vdmacs_s1








#define Q6_R_vdmpy_PP_rnd_sat __builtin_HEXAGON_M2_vdmpyrs_s0








#define Q6_R_vdmpy_PP_s1_rnd_sat __builtin_HEXAGON_M2_vdmpyrs_s1








#define Q6_P_vdmpy_PP_sat __builtin_HEXAGON_M2_vdmpys_s0








#define Q6_P_vdmpy_PP_s1_sat __builtin_HEXAGON_M2_vdmpys_s1








#define Q6_P_vmpyhacc_RR __builtin_HEXAGON_M2_vmac2








#define Q6_P_vmpyehacc_PP __builtin_HEXAGON_M2_vmac2es








#define Q6_P_vmpyehacc_PP_sat __builtin_HEXAGON_M2_vmac2es_s0








#define Q6_P_vmpyehacc_PP_s1_sat __builtin_HEXAGON_M2_vmac2es_s1








#define Q6_P_vmpyhacc_RR_sat __builtin_HEXAGON_M2_vmac2s_s0








#define Q6_P_vmpyhacc_RR_s1_sat __builtin_HEXAGON_M2_vmac2s_s1








#define Q6_P_vmpyhsuacc_RR_sat __builtin_HEXAGON_M2_vmac2su_s0








#define Q6_P_vmpyhsuacc_RR_s1_sat __builtin_HEXAGON_M2_vmac2su_s1








#define Q6_P_vmpyeh_PP_sat __builtin_HEXAGON_M2_vmpy2es_s0








#define Q6_P_vmpyeh_PP_s1_sat __builtin_HEXAGON_M2_vmpy2es_s1








#define Q6_P_vmpyh_RR_sat __builtin_HEXAGON_M2_vmpy2s_s0








#define Q6_R_vmpyh_RR_rnd_sat __builtin_HEXAGON_M2_vmpy2s_s0pack








#define Q6_P_vmpyh_RR_s1_sat __builtin_HEXAGON_M2_vmpy2s_s1








#define Q6_R_vmpyh_RR_s1_rnd_sat __builtin_HEXAGON_M2_vmpy2s_s1pack








#define Q6_P_vmpyhsu_RR_sat __builtin_HEXAGON_M2_vmpy2su_s0








#define Q6_P_vmpyhsu_RR_s1_sat __builtin_HEXAGON_M2_vmpy2su_s1








#define Q6_R_vraddh_PP __builtin_HEXAGON_M2_vraddh








#define Q6_R_vradduh_PP __builtin_HEXAGON_M2_vradduh








#define Q6_P_vrcmpyiacc_PP __builtin_HEXAGON_M2_vrcmaci_s0








#define Q6_P_vrcmpyiacc_PP_conj __builtin_HEXAGON_M2_vrcmaci_s0c








#define Q6_P_vrcmpyracc_PP __builtin_HEXAGON_M2_vrcmacr_s0








#define Q6_P_vrcmpyracc_PP_conj __builtin_HEXAGON_M2_vrcmacr_s0c








#define Q6_P_vrcmpyi_PP __builtin_HEXAGON_M2_vrcmpyi_s0








#define Q6_P_vrcmpyi_PP_conj __builtin_HEXAGON_M2_vrcmpyi_s0c








#define Q6_P_vrcmpyr_PP __builtin_HEXAGON_M2_vrcmpyr_s0








#define Q6_P_vrcmpyr_PP_conj __builtin_HEXAGON_M2_vrcmpyr_s0c








#define Q6_P_vrcmpysacc_PR_s1_sat __builtin_HEXAGON_M2_vrcmpys_acc_s1








#define Q6_P_vrcmpys_PR_s1_sat __builtin_HEXAGON_M2_vrcmpys_s1








#define Q6_R_vrcmpys_PR_s1_rnd_sat __builtin_HEXAGON_M2_vrcmpys_s1rp








#define Q6_P_vrmpyhacc_PP __builtin_HEXAGON_M2_vrmac_s0








#define Q6_P_vrmpyh_PP __builtin_HEXAGON_M2_vrmpy_s0








#define Q6_R_xorxacc_RR __builtin_HEXAGON_M2_xor_xacc








#define Q6_R_andand_RR __builtin_HEXAGON_M4_and_and








#define Q6_R_andand_RnR __builtin_HEXAGON_M4_and_andn








#define Q6_R_orand_RR __builtin_HEXAGON_M4_and_or








#define Q6_R_xorand_RR __builtin_HEXAGON_M4_and_xor








#define Q6_R_cmpyiwh_PR_s1_rnd_sat __builtin_HEXAGON_M4_cmpyi_wh








#define Q6_R_cmpyiwh_PR_conj_s1_rnd_sat __builtin_HEXAGON_M4_cmpyi_whc








#define Q6_R_cmpyrwh_PR_s1_rnd_sat __builtin_HEXAGON_M4_cmpyr_wh








#define Q6_R_cmpyrwh_PR_conj_s1_rnd_sat __builtin_HEXAGON_M4_cmpyr_whc








#define Q6_R_mpyacc_RR_s1_sat __builtin_HEXAGON_M4_mac_up_s1_sat








#define Q6_R_add_mpyi_IRI __builtin_HEXAGON_M4_mpyri_addi








#define Q6_R_add_mpyi_RRI __builtin_HEXAGON_M4_mpyri_addr








#define Q6_R_add_mpyi_RIR __builtin_HEXAGON_M4_mpyri_addr_u2








#define Q6_R_add_mpyi_IRR __builtin_HEXAGON_M4_mpyrr_addi








#define Q6_R_add_mpyi_RRR __builtin_HEXAGON_M4_mpyrr_addr








#define Q6_R_mpynac_RR_s1_sat __builtin_HEXAGON_M4_nac_up_s1_sat








#define Q6_R_andor_RR __builtin_HEXAGON_M4_or_and








#define Q6_R_andor_RnR __builtin_HEXAGON_M4_or_andn








#define Q6_R_oror_RR __builtin_HEXAGON_M4_or_or








#define Q6_R_xoror_RR __builtin_HEXAGON_M4_or_xor








#define Q6_P_pmpyw_RR __builtin_HEXAGON_M4_pmpyw








#define Q6_P_pmpywxacc_RR __builtin_HEXAGON_M4_pmpyw_acc








#define Q6_P_vpmpyh_RR __builtin_HEXAGON_M4_vpmpyh








#define Q6_P_vpmpyhxacc_RR __builtin_HEXAGON_M4_vpmpyh_acc








#define Q6_P_vrmpywehacc_PP __builtin_HEXAGON_M4_vrmpyeh_acc_s0








#define Q6_P_vrmpywehacc_PP_s1 __builtin_HEXAGON_M4_vrmpyeh_acc_s1








#define Q6_P_vrmpyweh_PP __builtin_HEXAGON_M4_vrmpyeh_s0








#define Q6_P_vrmpyweh_PP_s1 __builtin_HEXAGON_M4_vrmpyeh_s1








#define Q6_P_vrmpywohacc_PP __builtin_HEXAGON_M4_vrmpyoh_acc_s0








#define Q6_P_vrmpywohacc_PP_s1 __builtin_HEXAGON_M4_vrmpyoh_acc_s1








#define Q6_P_vrmpywoh_PP __builtin_HEXAGON_M4_vrmpyoh_s0








#define Q6_P_vrmpywoh_PP_s1 __builtin_HEXAGON_M4_vrmpyoh_s1








#define Q6_R_andxacc_RR __builtin_HEXAGON_M4_xor_and








#define Q6_R_andxacc_RnR __builtin_HEXAGON_M4_xor_andn








#define Q6_R_orxacc_RR __builtin_HEXAGON_M4_xor_or








#define Q6_P_xorxacc_PP __builtin_HEXAGON_M4_xor_xacc








#define Q6_P_vdmpybsuacc_PP_sat __builtin_HEXAGON_M5_vdmacbsu








#define Q6_P_vdmpybsu_PP_sat __builtin_HEXAGON_M5_vdmpybsu








#define Q6_P_vmpybsuacc_RR __builtin_HEXAGON_M5_vmacbsu








#define Q6_P_vmpybuacc_RR __builtin_HEXAGON_M5_vmacbuu








#define Q6_P_vmpybsu_RR __builtin_HEXAGON_M5_vmpybsu








#define Q6_P_vmpybu_RR __builtin_HEXAGON_M5_vmpybuu








#define Q6_P_vrmpybsuacc_PP __builtin_HEXAGON_M5_vrmacbsu








#define Q6_P_vrmpybuacc_PP __builtin_HEXAGON_M5_vrmacbuu








#define Q6_P_vrmpybsu_PP __builtin_HEXAGON_M5_vrmpybsu








#define Q6_P_vrmpybu_PP __builtin_HEXAGON_M5_vrmpybuu








#define Q6_R_addasl_RRI __builtin_HEXAGON_S2_addasl_rrri








#define Q6_P_asl_PI __builtin_HEXAGON_S2_asl_i_p








#define Q6_P_aslacc_PI __builtin_HEXAGON_S2_asl_i_p_acc








#define Q6_P_asland_PI __builtin_HEXAGON_S2_asl_i_p_and








#define Q6_P_aslnac_PI __builtin_HEXAGON_S2_asl_i_p_nac








#define Q6_P_aslor_PI __builtin_HEXAGON_S2_asl_i_p_or








#define Q6_P_aslxacc_PI __builtin_HEXAGON_S2_asl_i_p_xacc








#define Q6_R_asl_RI __builtin_HEXAGON_S2_asl_i_r








#define Q6_R_aslacc_RI __builtin_HEXAGON_S2_asl_i_r_acc








#define Q6_R_asland_RI __builtin_HEXAGON_S2_asl_i_r_and








#define Q6_R_aslnac_RI __builtin_HEXAGON_S2_asl_i_r_nac








#define Q6_R_aslor_RI __builtin_HEXAGON_S2_asl_i_r_or








#define Q6_R_asl_RI_sat __builtin_HEXAGON_S2_asl_i_r_sat








#define Q6_R_aslxacc_RI __builtin_HEXAGON_S2_asl_i_r_xacc








#define Q6_P_vaslh_PI __builtin_HEXAGON_S2_asl_i_vh








#define Q6_P_vaslw_PI __builtin_HEXAGON_S2_asl_i_vw








#define Q6_P_asl_PR __builtin_HEXAGON_S2_asl_r_p








#define Q6_P_aslacc_PR __builtin_HEXAGON_S2_asl_r_p_acc








#define Q6_P_asland_PR __builtin_HEXAGON_S2_asl_r_p_and








#define Q6_P_aslnac_PR __builtin_HEXAGON_S2_asl_r_p_nac








#define Q6_P_aslor_PR __builtin_HEXAGON_S2_asl_r_p_or








#define Q6_P_aslxacc_PR __builtin_HEXAGON_S2_asl_r_p_xor








#define Q6_R_asl_RR __builtin_HEXAGON_S2_asl_r_r








#define Q6_R_aslacc_RR __builtin_HEXAGON_S2_asl_r_r_acc








#define Q6_R_asland_RR __builtin_HEXAGON_S2_asl_r_r_and








#define Q6_R_aslnac_RR __builtin_HEXAGON_S2_asl_r_r_nac








#define Q6_R_aslor_RR __builtin_HEXAGON_S2_asl_r_r_or








#define Q6_R_asl_RR_sat __builtin_HEXAGON_S2_asl_r_r_sat








#define Q6_P_vaslh_PR __builtin_HEXAGON_S2_asl_r_vh








#define Q6_P_vaslw_PR __builtin_HEXAGON_S2_asl_r_vw








#define Q6_P_asr_PI __builtin_HEXAGON_S2_asr_i_p








#define Q6_P_asracc_PI __builtin_HEXAGON_S2_asr_i_p_acc








#define Q6_P_asrand_PI __builtin_HEXAGON_S2_asr_i_p_and








#define Q6_P_asrnac_PI __builtin_HEXAGON_S2_asr_i_p_nac








#define Q6_P_asror_PI __builtin_HEXAGON_S2_asr_i_p_or








#define Q6_P_asr_PI_rnd __builtin_HEXAGON_S2_asr_i_p_rnd








#define Q6_P_asrrnd_PI __builtin_HEXAGON_S2_asr_i_p_rnd_goodsyntax








#define Q6_R_asr_RI __builtin_HEXAGON_S2_asr_i_r








#define Q6_R_asracc_RI __builtin_HEXAGON_S2_asr_i_r_acc








#define Q6_R_asrand_RI __builtin_HEXAGON_S2_asr_i_r_and








#define Q6_R_asrnac_RI __builtin_HEXAGON_S2_asr_i_r_nac








#define Q6_R_asror_RI __builtin_HEXAGON_S2_asr_i_r_or








#define Q6_R_asr_RI_rnd __builtin_HEXAGON_S2_asr_i_r_rnd








#define Q6_R_asrrnd_RI __builtin_HEXAGON_S2_asr_i_r_rnd_goodsyntax








#define Q6_R_vasrw_PI __builtin_HEXAGON_S2_asr_i_svw_trun








#define Q6_P_vasrh_PI __builtin_HEXAGON_S2_asr_i_vh








#define Q6_P_vasrw_PI __builtin_HEXAGON_S2_asr_i_vw








#define Q6_P_asr_PR __builtin_HEXAGON_S2_asr_r_p








#define Q6_P_asracc_PR __builtin_HEXAGON_S2_asr_r_p_acc








#define Q6_P_asrand_PR __builtin_HEXAGON_S2_asr_r_p_and








#define Q6_P_asrnac_PR __builtin_HEXAGON_S2_asr_r_p_nac








#define Q6_P_asror_PR __builtin_HEXAGON_S2_asr_r_p_or








#define Q6_P_asrxacc_PR __builtin_HEXAGON_S2_asr_r_p_xor








#define Q6_R_asr_RR __builtin_HEXAGON_S2_asr_r_r








#define Q6_R_asracc_RR __builtin_HEXAGON_S2_asr_r_r_acc








#define Q6_R_asrand_RR __builtin_HEXAGON_S2_asr_r_r_and








#define Q6_R_asrnac_RR __builtin_HEXAGON_S2_asr_r_r_nac








#define Q6_R_asror_RR __builtin_HEXAGON_S2_asr_r_r_or








#define Q6_R_asr_RR_sat __builtin_HEXAGON_S2_asr_r_r_sat








#define Q6_R_vasrw_PR __builtin_HEXAGON_S2_asr_r_svw_trun








#define Q6_P_vasrh_PR __builtin_HEXAGON_S2_asr_r_vh








#define Q6_P_vasrw_PR __builtin_HEXAGON_S2_asr_r_vw








#define Q6_R_brev_R __builtin_HEXAGON_S2_brev








#define Q6_P_brev_P __builtin_HEXAGON_S2_brevp








#define Q6_R_cl0_R __builtin_HEXAGON_S2_cl0








#define Q6_R_cl0_P __builtin_HEXAGON_S2_cl0p








#define Q6_R_cl1_R __builtin_HEXAGON_S2_cl1








#define Q6_R_cl1_P __builtin_HEXAGON_S2_cl1p








#define Q6_R_clb_R __builtin_HEXAGON_S2_clb








#define Q6_R_normamt_R __builtin_HEXAGON_S2_clbnorm








#define Q6_R_clb_P __builtin_HEXAGON_S2_clbp








#define Q6_R_clrbit_RI __builtin_HEXAGON_S2_clrbit_i








#define Q6_R_clrbit_RR __builtin_HEXAGON_S2_clrbit_r








#define Q6_R_ct0_R __builtin_HEXAGON_S2_ct0








#define Q6_R_ct0_P __builtin_HEXAGON_S2_ct0p








#define Q6_R_ct1_R __builtin_HEXAGON_S2_ct1








#define Q6_R_ct1_P __builtin_HEXAGON_S2_ct1p








#define Q6_P_deinterleave_P __builtin_HEXAGON_S2_deinterleave








#define Q6_R_extractu_RII __builtin_HEXAGON_S2_extractu








#define Q6_R_extractu_RP __builtin_HEXAGON_S2_extractu_rp








#define Q6_P_extractu_PII __builtin_HEXAGON_S2_extractup








#define Q6_P_extractu_PP __builtin_HEXAGON_S2_extractup_rp








#define Q6_R_insert_RII __builtin_HEXAGON_S2_insert








#define Q6_R_insert_RP __builtin_HEXAGON_S2_insert_rp








#define Q6_P_insert_PII __builtin_HEXAGON_S2_insertp








#define Q6_P_insert_PP __builtin_HEXAGON_S2_insertp_rp








#define Q6_P_interleave_P __builtin_HEXAGON_S2_interleave








#define Q6_P_lfs_PP __builtin_HEXAGON_S2_lfsp








#define Q6_P_lsl_PR __builtin_HEXAGON_S2_lsl_r_p








#define Q6_P_lslacc_PR __builtin_HEXAGON_S2_lsl_r_p_acc








#define Q6_P_lsland_PR __builtin_HEXAGON_S2_lsl_r_p_and








#define Q6_P_lslnac_PR __builtin_HEXAGON_S2_lsl_r_p_nac








#define Q6_P_lslor_PR __builtin_HEXAGON_S2_lsl_r_p_or








#define Q6_P_lslxacc_PR __builtin_HEXAGON_S2_lsl_r_p_xor








#define Q6_R_lsl_RR __builtin_HEXAGON_S2_lsl_r_r








#define Q6_R_lslacc_RR __builtin_HEXAGON_S2_lsl_r_r_acc








#define Q6_R_lsland_RR __builtin_HEXAGON_S2_lsl_r_r_and








#define Q6_R_lslnac_RR __builtin_HEXAGON_S2_lsl_r_r_nac








#define Q6_R_lslor_RR __builtin_HEXAGON_S2_lsl_r_r_or








#define Q6_P_vlslh_PR __builtin_HEXAGON_S2_lsl_r_vh








#define Q6_P_vlslw_PR __builtin_HEXAGON_S2_lsl_r_vw








#define Q6_P_lsr_PI __builtin_HEXAGON_S2_lsr_i_p








#define Q6_P_lsracc_PI __builtin_HEXAGON_S2_lsr_i_p_acc








#define Q6_P_lsrand_PI __builtin_HEXAGON_S2_lsr_i_p_and








#define Q6_P_lsrnac_PI __builtin_HEXAGON_S2_lsr_i_p_nac








#define Q6_P_lsror_PI __builtin_HEXAGON_S2_lsr_i_p_or








#define Q6_P_lsrxacc_PI __builtin_HEXAGON_S2_lsr_i_p_xacc








#define Q6_R_lsr_RI __builtin_HEXAGON_S2_lsr_i_r








#define Q6_R_lsracc_RI __builtin_HEXAGON_S2_lsr_i_r_acc








#define Q6_R_lsrand_RI __builtin_HEXAGON_S2_lsr_i_r_and








#define Q6_R_lsrnac_RI __builtin_HEXAGON_S2_lsr_i_r_nac








#define Q6_R_lsror_RI __builtin_HEXAGON_S2_lsr_i_r_or








#define Q6_R_lsrxacc_RI __builtin_HEXAGON_S2_lsr_i_r_xacc








#define Q6_P_vlsrh_PI __builtin_HEXAGON_S2_lsr_i_vh








#define Q6_P_vlsrw_PI __builtin_HEXAGON_S2_lsr_i_vw








#define Q6_P_lsr_PR __builtin_HEXAGON_S2_lsr_r_p








#define Q6_P_lsracc_PR __builtin_HEXAGON_S2_lsr_r_p_acc








#define Q6_P_lsrand_PR __builtin_HEXAGON_S2_lsr_r_p_and








#define Q6_P_lsrnac_PR __builtin_HEXAGON_S2_lsr_r_p_nac








#define Q6_P_lsror_PR __builtin_HEXAGON_S2_lsr_r_p_or








#define Q6_P_lsrxacc_PR __builtin_HEXAGON_S2_lsr_r_p_xor








#define Q6_R_lsr_RR __builtin_HEXAGON_S2_lsr_r_r








#define Q6_R_lsracc_RR __builtin_HEXAGON_S2_lsr_r_r_acc








#define Q6_R_lsrand_RR __builtin_HEXAGON_S2_lsr_r_r_and








#define Q6_R_lsrnac_RR __builtin_HEXAGON_S2_lsr_r_r_nac








#define Q6_R_lsror_RR __builtin_HEXAGON_S2_lsr_r_r_or








#define Q6_P_vlsrh_PR __builtin_HEXAGON_S2_lsr_r_vh








#define Q6_P_vlsrw_PR __builtin_HEXAGON_S2_lsr_r_vw








#define Q6_P_packhl_RR __builtin_HEXAGON_S2_packhl








#define Q6_R_parity_PP __builtin_HEXAGON_S2_parityp








#define Q6_R_setbit_RI __builtin_HEXAGON_S2_setbit_i








#define Q6_R_setbit_RR __builtin_HEXAGON_S2_setbit_r








#define Q6_P_shuffeb_PP __builtin_HEXAGON_S2_shuffeb








#define Q6_P_shuffeh_PP __builtin_HEXAGON_S2_shuffeh








#define Q6_P_shuffob_PP __builtin_HEXAGON_S2_shuffob








#define Q6_P_shuffoh_PP __builtin_HEXAGON_S2_shuffoh








#define Q6_memb_IMR_circ __builtin_HEXAGON_S2_storerb_pci








#define Q6_memb_MR_circ __builtin_HEXAGON_S2_storerb_pcr








#define Q6_memd_IMP_circ __builtin_HEXAGON_S2_storerd_pci








#define Q6_memd_MP_circ __builtin_HEXAGON_S2_storerd_pcr








#define Q6_memh_IMRh_circ __builtin_HEXAGON_S2_storerf_pci








#define Q6_memh_MRh_circ __builtin_HEXAGON_S2_storerf_pcr








#define Q6_memh_IMR_circ __builtin_HEXAGON_S2_storerh_pci








#define Q6_memh_MR_circ __builtin_HEXAGON_S2_storerh_pcr








#define Q6_memw_IMR_circ __builtin_HEXAGON_S2_storeri_pci








#define Q6_memw_MR_circ __builtin_HEXAGON_S2_storeri_pcr








#define Q6_R_vsathb_R __builtin_HEXAGON_S2_svsathb








#define Q6_R_vsathub_R __builtin_HEXAGON_S2_svsathub








#define Q6_R_tableidxb_RII __builtin_HEXAGON_S2_tableidxb_goodsyntax








#define Q6_R_tableidxd_RII __builtin_HEXAGON_S2_tableidxd_goodsyntax








#define Q6_R_tableidxh_RII __builtin_HEXAGON_S2_tableidxh_goodsyntax








#define Q6_R_tableidxw_RII __builtin_HEXAGON_S2_tableidxw_goodsyntax








#define Q6_R_togglebit_RI __builtin_HEXAGON_S2_togglebit_i








#define Q6_R_togglebit_RR __builtin_HEXAGON_S2_togglebit_r








#define Q6_p_tstbit_RI __builtin_HEXAGON_S2_tstbit_i








#define Q6_p_tstbit_RR __builtin_HEXAGON_S2_tstbit_r








#define Q6_P_valignb_PPI __builtin_HEXAGON_S2_valignib








#define Q6_P_valignb_PPp __builtin_HEXAGON_S2_valignrb








#define Q6_P_vcnegh_PR __builtin_HEXAGON_S2_vcnegh








#define Q6_P_vcrotate_PR __builtin_HEXAGON_S2_vcrotate








#define Q6_P_vrcneghacc_PR __builtin_HEXAGON_S2_vrcnegh








#define Q6_R_vrndwh_P __builtin_HEXAGON_S2_vrndpackwh








#define Q6_R_vrndwh_P_sat __builtin_HEXAGON_S2_vrndpackwhs








#define Q6_R_vsathb_P __builtin_HEXAGON_S2_vsathb








#define Q6_P_vsathb_P __builtin_HEXAGON_S2_vsathb_nopack








#define Q6_R_vsathub_P __builtin_HEXAGON_S2_vsathub








#define Q6_P_vsathub_P __builtin_HEXAGON_S2_vsathub_nopack








#define Q6_R_vsatwh_P __builtin_HEXAGON_S2_vsatwh








#define Q6_P_vsatwh_P __builtin_HEXAGON_S2_vsatwh_nopack








#define Q6_R_vsatwuh_P __builtin_HEXAGON_S2_vsatwuh








#define Q6_P_vsatwuh_P __builtin_HEXAGON_S2_vsatwuh_nopack








#define Q6_R_vsplatb_R __builtin_HEXAGON_S2_vsplatrb








#define Q6_P_vsplath_R __builtin_HEXAGON_S2_vsplatrh








#define Q6_P_vspliceb_PPI __builtin_HEXAGON_S2_vspliceib








#define Q6_P_vspliceb_PPp __builtin_HEXAGON_S2_vsplicerb








#define Q6_P_vsxtbh_R __builtin_HEXAGON_S2_vsxtbh








#define Q6_P_vsxthw_R __builtin_HEXAGON_S2_vsxthw








#define Q6_R_vtrunehb_P __builtin_HEXAGON_S2_vtrunehb








#define Q6_P_vtrunewh_PP __builtin_HEXAGON_S2_vtrunewh








#define Q6_R_vtrunohb_P __builtin_HEXAGON_S2_vtrunohb








#define Q6_P_vtrunowh_PP __builtin_HEXAGON_S2_vtrunowh








#define Q6_P_vzxtbh_R __builtin_HEXAGON_S2_vzxtbh








#define Q6_P_vzxthw_R __builtin_HEXAGON_S2_vzxthw








#define Q6_R_add_add_RRI __builtin_HEXAGON_S4_addaddi








#define Q6_R_add_asl_IRI __builtin_HEXAGON_S4_addi_asl_ri








#define Q6_R_add_lsr_IRI __builtin_HEXAGON_S4_addi_lsr_ri








#define Q6_R_and_asl_IRI __builtin_HEXAGON_S4_andi_asl_ri








#define Q6_R_and_lsr_IRI __builtin_HEXAGON_S4_andi_lsr_ri








#define Q6_R_add_clb_RI __builtin_HEXAGON_S4_clbaddi








#define Q6_R_add_clb_PI __builtin_HEXAGON_S4_clbpaddi








#define Q6_R_normamt_P __builtin_HEXAGON_S4_clbpnorm








#define Q6_R_extract_RII __builtin_HEXAGON_S4_extract








#define Q6_R_extract_RP __builtin_HEXAGON_S4_extract_rp








#define Q6_P_extract_PII __builtin_HEXAGON_S4_extractp








#define Q6_P_extract_PP __builtin_HEXAGON_S4_extractp_rp








#define Q6_R_lsl_IR __builtin_HEXAGON_S4_lsli








#define Q6_p_not_tstbit_RI __builtin_HEXAGON_S4_ntstbit_i








#define Q6_p_not_tstbit_RR __builtin_HEXAGON_S4_ntstbit_r








#define Q6_R_andor_RI __builtin_HEXAGON_S4_or_andi








#define Q6_R_or_and_RRI __builtin_HEXAGON_S4_or_andix








#define Q6_R_oror_RI __builtin_HEXAGON_S4_or_ori








#define Q6_R_or_asl_IRI __builtin_HEXAGON_S4_ori_asl_ri








#define Q6_R_or_lsr_IRI __builtin_HEXAGON_S4_ori_lsr_ri








#define Q6_R_parity_RR __builtin_HEXAGON_S4_parity








#define Q6_R_add_sub_RIR __builtin_HEXAGON_S4_subaddi








#define Q6_R_sub_asl_IRI __builtin_HEXAGON_S4_subi_asl_ri








#define Q6_R_sub_lsr_IRI __builtin_HEXAGON_S4_subi_lsr_ri








#define Q6_P_vrcrotate_PRI __builtin_HEXAGON_S4_vrcrotate








#define Q6_P_vrcrotateacc_PRI __builtin_HEXAGON_S4_vrcrotate_acc








#define Q6_P_vxaddsubh_PP_sat __builtin_HEXAGON_S4_vxaddsubh








#define Q6_P_vxaddsubh_PP_rnd_rs1_sat __builtin_HEXAGON_S4_vxaddsubhr








#define Q6_P_vxaddsubw_PP_sat __builtin_HEXAGON_S4_vxaddsubw








#define Q6_P_vxsubaddh_PP_sat __builtin_HEXAGON_S4_vxsubaddh








#define Q6_P_vxsubaddh_PP_rnd_rs1_sat __builtin_HEXAGON_S4_vxsubaddhr








#define Q6_P_vxsubaddw_PP_sat __builtin_HEXAGON_S4_vxsubaddw








#define Q6_R_vasrhub_PI_rnd_sat __builtin_HEXAGON_S5_asrhub_rnd_sat_goodsyntax








#define Q6_R_vasrhub_PI_sat __builtin_HEXAGON_S5_asrhub_sat








#define Q6_R_popcount_P __builtin_HEXAGON_S5_popcountp








#define Q6_P_vasrh_PI_rnd __builtin_HEXAGON_S5_vasrhrnd_goodsyntax








#define Q6_dccleana_A __builtin_HEXAGON_Y2_dccleana








#define Q6_dccleaninva_A __builtin_HEXAGON_Y2_dccleaninva








#define Q6_dcfetch_A __builtin_HEXAGON_Y2_dcfetch








#define Q6_dcinva_A __builtin_HEXAGON_Y2_dcinva








#define Q6_dczeroa_A __builtin_HEXAGON_Y2_dczeroa








#define Q6_l2fetch_AR __builtin_HEXAGON_Y4_l2fetch








#define Q6_l2fetch_AP __builtin_HEXAGON_Y5_l2fetch

#if __HEXAGON_ARCH__ >= 60







#define Q6_P_rol_PI __builtin_HEXAGON_S6_rol_i_p
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_P_rolacc_PI __builtin_HEXAGON_S6_rol_i_p_acc
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_P_roland_PI __builtin_HEXAGON_S6_rol_i_p_and
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_P_rolnac_PI __builtin_HEXAGON_S6_rol_i_p_nac
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_P_rolor_PI __builtin_HEXAGON_S6_rol_i_p_or
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_P_rolxacc_PI __builtin_HEXAGON_S6_rol_i_p_xacc
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_R_rol_RI __builtin_HEXAGON_S6_rol_i_r
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_R_rolacc_RI __builtin_HEXAGON_S6_rol_i_r_acc
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_R_roland_RI __builtin_HEXAGON_S6_rol_i_r_and
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_R_rolnac_RI __builtin_HEXAGON_S6_rol_i_r_nac
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_R_rolor_RI __builtin_HEXAGON_S6_rol_i_r_or
#endif

#if __HEXAGON_ARCH__ >= 60







#define Q6_R_rolxacc_RI __builtin_HEXAGON_S6_rol_i_r_xacc
#endif

#if __HEXAGON_ARCH__ >= 62







#define Q6_P_vabsdiffb_PP __builtin_HEXAGON_M6_vabsdiffb
#endif

#if __HEXAGON_ARCH__ >= 62







#define Q6_P_vabsdiffub_PP __builtin_HEXAGON_M6_vabsdiffub
#endif

#if __HEXAGON_ARCH__ >= 62







#define Q6_P_vsplatb_R __builtin_HEXAGON_S6_vsplatrbp
#endif

#if __HEXAGON_ARCH__ >= 62







#define Q6_P_vtrunehb_PP __builtin_HEXAGON_S6_vtrunehb_ppp
#endif

#if __HEXAGON_ARCH__ >= 62







#define Q6_P_vtrunohb_PP __builtin_HEXAGON_S6_vtrunohb_ppp
#endif

#if __HEXAGON_ARCH__ >= 62







#define Q6_V_vmem_R_nt __builtin_HEXAGON_V6_ldntnt0
#endif

#if __HEXAGON_ARCH__ >= 65







#define Q6_p_not_any8_vcmpb_eq_PP __builtin_HEXAGON_A6_vcmpbeq_notany
#endif

#if __HEXAGON_ARCH__ >= 66







#define Q6_P_dfadd_PP __builtin_HEXAGON_F2_dfadd
#endif

#if __HEXAGON_ARCH__ >= 66







#define Q6_P_dfsub_PP __builtin_HEXAGON_F2_dfsub
#endif

#if __HEXAGON_ARCH__ >= 66







#define Q6_R_mpyinac_RR __builtin_HEXAGON_M2_mnaci
#endif

#if __HEXAGON_ARCH__ >= 66







#define Q6_R_mask_II __builtin_HEXAGON_S2_mask
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_R_clip_RI __builtin_HEXAGON_A7_clip
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_cround_PI __builtin_HEXAGON_A7_croundd_ri
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_cround_PR __builtin_HEXAGON_A7_croundd_rr
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_vclip_PI __builtin_HEXAGON_A7_vclip
#endif

#if __HEXAGON_ARCH__ >= 67







#define Q6_P_dfmax_PP __builtin_HEXAGON_F2_dfmax
#endif

#if __HEXAGON_ARCH__ >= 67







#define Q6_P_dfmin_PP __builtin_HEXAGON_F2_dfmin
#endif

#if __HEXAGON_ARCH__ >= 67







#define Q6_P_dfmpyfix_PP __builtin_HEXAGON_F2_dfmpyfix
#endif

#if __HEXAGON_ARCH__ >= 67







#define Q6_P_dfmpyhhacc_PP __builtin_HEXAGON_F2_dfmpyhh
#endif

#if __HEXAGON_ARCH__ >= 67







#define Q6_P_dfmpylhacc_PP __builtin_HEXAGON_F2_dfmpylh
#endif

#if __HEXAGON_ARCH__ >= 67







#define Q6_P_dfmpyll_PP __builtin_HEXAGON_F2_dfmpyll
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_cmpyiw_PP __builtin_HEXAGON_M7_dcmpyiw
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_cmpyiwacc_PP __builtin_HEXAGON_M7_dcmpyiw_acc
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_cmpyiw_PP_conj __builtin_HEXAGON_M7_dcmpyiwc
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_cmpyiwacc_PP_conj __builtin_HEXAGON_M7_dcmpyiwc_acc
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_cmpyrw_PP __builtin_HEXAGON_M7_dcmpyrw
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_cmpyrwacc_PP __builtin_HEXAGON_M7_dcmpyrw_acc
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_cmpyrw_PP_conj __builtin_HEXAGON_M7_dcmpyrwc
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_cmpyrwacc_PP_conj __builtin_HEXAGON_M7_dcmpyrwc_acc
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_vdmpyw_PP __builtin_HEXAGON_M7_vdmpy
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_P_vdmpywacc_PP __builtin_HEXAGON_M7_vdmpy_acc
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_R_cmpyiw_PP_s1_sat __builtin_HEXAGON_M7_wcmpyiw
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_R_cmpyiw_PP_s1_rnd_sat __builtin_HEXAGON_M7_wcmpyiw_rnd
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_R_cmpyiw_PP_conj_s1_sat __builtin_HEXAGON_M7_wcmpyiwc
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_R_cmpyiw_PP_conj_s1_rnd_sat __builtin_HEXAGON_M7_wcmpyiwc_rnd
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_R_cmpyrw_PP_s1_sat __builtin_HEXAGON_M7_wcmpyrw
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_R_cmpyrw_PP_s1_rnd_sat __builtin_HEXAGON_M7_wcmpyrw_rnd
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_R_cmpyrw_PP_conj_s1_sat __builtin_HEXAGON_M7_wcmpyrwc
#endif

#if __HEXAGON_ARCH__ >= 67 && defined __HEXAGON_AUDIO__







#define Q6_R_cmpyrw_PP_conj_s1_rnd_sat __builtin_HEXAGON_M7_wcmpyrwc_rnd
#endif

#if __HEXAGON_ARCH__ >= 68







#define Q6_dmlink_AA __builtin_HEXAGON_Y6_dmlink
#endif

#if __HEXAGON_ARCH__ >= 68







#define Q6_R_dmpause __builtin_HEXAGON_Y6_dmpause
#endif

#if __HEXAGON_ARCH__ >= 68







#define Q6_R_dmpoll __builtin_HEXAGON_Y6_dmpoll
#endif

#if __HEXAGON_ARCH__ >= 68







#define Q6_dmresume_A __builtin_HEXAGON_Y6_dmresume
#endif

#if __HEXAGON_ARCH__ >= 68







#define Q6_dmstart_A __builtin_HEXAGON_Y6_dmstart
#endif

#if __HEXAGON_ARCH__ >= 68







#define Q6_R_dmwait __builtin_HEXAGON_Y6_dmwait
#endif

#include <hexagon_circ_brev_intrinsics.h>
#if defined(__HVX__)
#include <hvx_hexagon_protos.h>
#endif
#endif
