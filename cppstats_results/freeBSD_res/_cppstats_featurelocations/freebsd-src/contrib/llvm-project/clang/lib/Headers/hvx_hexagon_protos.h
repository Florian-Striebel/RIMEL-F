











#if !defined(_HVX_HEXAGON_PROTOS_H_)
#define _HVX_HEXAGON_PROTOS_H_ 1

#if defined(__HVX__)
#if __HVX_LENGTH__ == 128
#define __BUILTIN_VECTOR_WRAP(a) a ##_128B
#else
#define __BUILTIN_VECTOR_WRAP(a) a
#endif

#if __HVX_ARCH__ >= 60







#define Q6_R_vextract_VR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_extractw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_hi_W __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_hi)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_lo_W __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_lo)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vsplat_R __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_lvsplatw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_and_QQ __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_and)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_and_QQn __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_and_n)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_not_Q __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_not)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_or_QQ __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_or)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_or_QQn __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_or_n)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vsetq_R __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_scalar2)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_xor_QQ __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_xor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_vmem_QnRIV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vS32b_nqpred_ai)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_vmem_QnRIV_nt __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vS32b_nt_nqpred_ai)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_vmem_QRIV_nt __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vS32b_nt_qpred_ai)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_vmem_QRIV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vS32b_qpred_ai)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vabsdiff_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsdiffh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vabsdiff_VubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsdiffub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vabsdiff_VuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsdiffuh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuw_vabsdiff_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsdiffw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vabs_Vh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vabs_Vh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsh_sat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vabs_Vw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vabs_Vw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsw_sat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vadd_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wb_vadd_WbWb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddb_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_condacc_QnVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddbnq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_condacc_QVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddbq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vadd_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vadd_WhWh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddh_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_condacc_QnVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhnq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_condacc_QVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vadd_VhVh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vadd_WhWh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhsat_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vadd_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vadd_VubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddubh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vadd_VubVub_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddubsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wub_vadd_WubWub_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddubsat_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vadd_VuhVuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vadduhsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuh_vadd_WuhWuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vadduhsat_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vadd_VuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vadduhw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vadd_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vadd_WwWw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddw_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_condacc_QnVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddwnq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_condacc_QVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddwq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vadd_VwVw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddwsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vadd_WwWw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddwsat_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_valign_VVR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_valignb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_valign_VVI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_valignbi)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vand_VV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vand)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vand_QR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vandor_VQR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandqrt_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vand_VR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vandor_QVR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvrt_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vasl_VhR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vasl_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslhv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vasl_VwR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vaslacc_VwVwR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslw_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vasl_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslwv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vasr_VhR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vasr_VhVhR_rnd_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrhbrndsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vasr_VhVhR_rnd_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrhubrndsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vasr_VhVhR_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrhubsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vasr_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrhv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vasr_VwR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vasracc_VwVwR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrw_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vasr_VwVwR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vasr_VwVwR_rnd_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwhrndsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vasr_VwVwR_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwhsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vasr_VwVwR_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwuhsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vasr_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_equals_V __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vassign)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_W_equals_W __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vassignp)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vavg_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vavg_VhVh_rnd __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavghrnd)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vavg_VubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vavg_VubVub_rnd __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgubrnd)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vavg_VuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavguh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vavg_VuhVuh_rnd __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavguhrnd)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vavg_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vavg_VwVw_rnd __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgwrnd)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vcl0_Vuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vcl0h)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuw_vcl0_Vuw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vcl0w)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_W_vcombine_VV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vcombine)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vzero __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vd0)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vdeal_Vb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdealb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vdeale_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdealb4w)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vdeal_Vh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdealh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_W_vdeal_VVR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdealvdd)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vdelta_VV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdelta)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vdmpy_VubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpybus)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vdmpyacc_VhVubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpybus_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vdmpy_WubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpybus_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vdmpyacc_WhWubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpybus_dv_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpy_VhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpyacc_VwVhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhb_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vdmpy_WhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhb_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vdmpyacc_WwWhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhb_dv_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpy_WhRh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhisat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpyacc_VwWhRh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhisat_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpy_VhRh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpyacc_VwVhRh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsat_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpy_WhRuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsuisat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpyacc_VwWhRuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsuisat_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpy_VhRuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsusat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpyacc_VwVhRuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhsusat_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpy_VhVh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhvsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vdmpyacc_VwVhVh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdmpyhvsat_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vdsad_WuhRuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdsaduh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vdsadacc_WuwWuhRuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdsaduh_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eq_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eqand_QVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqb_and)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eqor_QVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqb_or)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eqxacc_QVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqb_xor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eq_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eqand_QVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqh_and)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eqor_QVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqh_or)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eqxacc_QVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqh_xor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eq_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eqand_QVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqw_and)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eqor_QVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqw_or)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_eqxacc_QVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_veqw_xor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gt_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtand_QVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtb_and)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtor_QVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtb_or)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtxacc_QVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtb_xor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gt_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgth)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtand_QVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgth_and)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtor_QVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgth_or)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtxacc_QVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgth_xor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gt_VubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtand_QVubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtub_and)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtor_QVubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtub_or)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtxacc_QVubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtub_xor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gt_VuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtand_QVuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuh_and)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtor_QVuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuh_or)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtxacc_QVuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuh_xor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gt_VuwVuw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtand_QVuwVuw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuw_and)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtor_QVuwVuw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuw_or)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtxacc_QVuwVuw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtuw_xor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gt_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtand_QVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtw_and)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtor_QVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtw_or)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Q_vcmp_gtxacc_QVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgtw_xor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vinsert_VwR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vinsertwr)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vlalign_VVR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlalignb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vlalign_VVI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlalignbi)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vlsr_VuhR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlsrh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vlsr_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlsrhv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuw_vlsr_VuwR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlsrw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vlsr_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlsrwv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vlut32_VbVbR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvvb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vlut32or_VbVbVbR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvvb_oracc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vlut16_VbVhR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvwh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vlut16or_WhVbVhR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvwh_oracc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vmax_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmaxh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vmax_VubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmaxub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vmax_VuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmaxuh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmax_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmaxw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vmin_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vminh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vmin_VubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vminub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vmin_VuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vminuh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmin_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vminw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vmpa_WubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpabus)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vmpaacc_WhWubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpabus_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vmpa_WubWb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpabusv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vmpa_WubWub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpabuuv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vmpa_WhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpahb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vmpaacc_WwWhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpahb_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vmpy_VubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpybus)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vmpyacc_WhVubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpybus_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vmpy_VubVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpybusv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vmpyacc_WhVubVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpybusv_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vmpy_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpybv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vmpyacc_WhVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpybv_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpye_VwVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyewuh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vmpy_VhRh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vmpyacc_WwVhRh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyhsat_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vmpy_VhRh_s1_rnd_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyhsrs)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vmpy_VhRh_s1_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyhss)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vmpy_VhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyhus)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vmpyacc_WwVhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyhus_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vmpy_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyhv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vmpyacc_WwVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyhv_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vmpy_VhVh_s1_rnd_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyhvsrs)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyieo_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyieoh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyieacc_VwVwVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyiewh_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyie_VwVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyiewuh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyieacc_VwVwVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyiewuh_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vmpyi_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyih)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vmpyiacc_VhVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyih_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vmpyi_VhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyihb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vmpyiacc_VhVhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyihb_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyio_VwVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyiowh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyi_VwRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyiwb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyiacc_VwVwRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyiwb_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyi_VwRh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyiwh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyiacc_VwVwRh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyiwh_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyo_VwVh_s1_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyowh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyo_VwVh_s1_rnd_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyowh_rnd)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyoacc_VwVwVh_s1_rnd_sat_shift __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyowh_rnd_sacc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vmpyoacc_VwVwVh_s1_sat_shift __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyowh_sacc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuh_vmpy_VubRub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuh_vmpyacc_WuhVubRub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyub_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuh_vmpy_VubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyubv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuh_vmpyacc_WuhVubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyubv_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vmpy_VuhRuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyuh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vmpyacc_WuwVuhRuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyuh_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vmpy_VuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyuhv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vmpyacc_WuwVuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyuhv_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vmux_QVV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmux)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vnavg_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vnavgh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vnavg_VubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vnavgub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vnavg_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vnavgw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vnormamt_Vh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vnormamth)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vnormamt_Vw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vnormamtw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vnot_V __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vnot)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vor_VV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vpacke_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vpackeb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vpacke_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vpackeh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vpack_VhVh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vpackhb_sat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vpack_VhVh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vpackhub_sat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vpacko_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vpackob)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vpacko_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vpackoh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vpack_VwVw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vpackwh_sat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vpack_VwVw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vpackwuh_sat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vpopcount_Vh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vpopcounth)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vrdelta_VV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrdelta)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vrmpy_VubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpybus)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vrmpyacc_VwVubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpybus_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vrmpy_WubRbI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpybusi)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vrmpyacc_WwWubRbI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpybusi_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vrmpy_VubVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpybusv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vrmpyacc_VwVubVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpybusv_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vrmpy_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpybv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vrmpyacc_VwVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpybv_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuw_vrmpy_VubRub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpyub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuw_vrmpyacc_VuwVubRub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpyub_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vrmpy_WubRubI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpyubi)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vrmpyacc_WuwWubRubI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpyubi_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuw_vrmpy_VubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpyubv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuw_vrmpyacc_VuwVubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrmpyubv_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vror_VR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vror)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vround_VhVh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vroundhb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vround_VhVh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vroundhub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vround_VwVw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vroundwh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vround_VwVw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vroundwuh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vrsad_WubRubI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrsadubi)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vrsadacc_WuwWubRubI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrsadubi_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vsat_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsathub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vsat_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsatwh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vsxt_Vb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vsxt_Vh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vshuffe_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vshufeh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vshuff_Vb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vshuffb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vshuffe_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vshuffeb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vshuff_Vh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vshuffh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vshuffo_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vshuffob)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_W_vshuff_VVR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vshuffvdd)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wb_vshuffoe_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vshufoeb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vshuffoe_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vshufoeh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vshuffo_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vshufoh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_vsub_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wb_vsub_WbWb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubb_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_condnac_QnVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubbnq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vb_condnac_QVbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubbq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vsub_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vsub_WhWh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubh_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_condnac_QnVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubhnq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_condnac_QVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubhq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vh_vsub_VhVh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubhsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vsub_WhWh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubhsat_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vsub_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubhw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vsub_VubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsububh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vub_vsub_VubVub_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsububsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wub_vsub_WubWub_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsububsat_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vuh_vsub_VuhVuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubuhsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuh_vsub_WuhWuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubuhsat_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vsub_VuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubuhw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vsub_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubw)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vsub_WwWw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubw_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_condnac_QnVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubwnq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_condnac_QVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubwq)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Vw_vsub_VwVw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubwsat)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vsub_WwWw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubwsat_dv)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_W_vswap_QVV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vswap)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vtmpy_WbRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vtmpyb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vtmpyacc_WhWbRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vtmpyb_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vtmpy_WubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vtmpybus)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vtmpyacc_WhWubRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vtmpybus_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vtmpy_WhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vtmpyhb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vtmpyacc_WwWhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vtmpyhb_acc)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vunpack_Vb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vunpackb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vunpack_Vh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vunpackh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wh_vunpackoor_WhVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vunpackob)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Ww_vunpackoor_WwVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vunpackoh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuh_vunpack_Vub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vunpackub)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vunpack_Vuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vunpackuh)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_V_vxor_VV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vxor)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuh_vzxt_Vub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vzb)
#endif

#if __HVX_ARCH__ >= 60







#define Q6_Wuw_vzxt_Vuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vzh)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vb_vsplat_R __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_lvsplatb)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vh_vsplat_R __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_lvsplath)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Q_vsetq2_R __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_pred_scalar2v2)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Qb_vshuffe_QhQh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_shuffeqh)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Qh_vshuffe_QwQw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_shuffeqw)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vb_vadd_VbVb_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddbsat)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Wb_vadd_WbWb_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddbsat_dv)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vw_vadd_VwVwQ_carry __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddcarry)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vh_vadd_vclb_VhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddclbh)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vw_vadd_vclb_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddclbw)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Ww_vaddacc_WwVhVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddhw_acc)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Wh_vaddacc_WhVubVub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddubh_acc)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vub_vadd_VubVb_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddububb_sat)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Ww_vaddacc_WwVuhVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vadduhw_acc)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vuw_vadd_VuwVuw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vadduwsat)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Wuw_vadd_WuwWuw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vadduwsat_dv)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_V_vand_QnR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandnqrt)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_V_vandor_VQnR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandnqrt_acc)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_V_vand_QnV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvnqv)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_V_vand_QV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vandvqv)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vb_vasr_VhVhR_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrhbsat)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vuh_vasr_VuwVuwR_rnd_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasruwuhrndsat)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vuh_vasr_VwVwR_rnd_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrwuhrndsat)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vub_vlsr_VubR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlsrb)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vb_vlut32_VbVbR_nomatch __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvvb_nm)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vb_vlut32or_VbVbVbI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvvb_oracci)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vb_vlut32_VbVbI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvvbi)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Wh_vlut16_VbVhR_nomatch __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvwh_nm)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Wh_vlut16or_WhVbVhI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvwh_oracci)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Wh_vlut16_VbVhI __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlutvwhi)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vb_vmax_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmaxb)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vb_vmin_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vminb)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Ww_vmpa_WuhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpauhb)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Ww_vmpaacc_WwWuhRb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpauhb_acc)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_W_vmpye_VwVuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyewuh_64)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vw_vmpyi_VwRub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyiwub)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vw_vmpyiacc_VwVwRub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyiwub_acc)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_W_vmpyoacc_WVwVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyowh_64_acc)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vub_vround_VuhVuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrounduhub)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vuh_vround_VuwVuw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrounduwuh)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vuh_vsat_VuwVuw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsatuwuh)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vb_vsub_VbVb_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubbsat)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Wb_vsub_WbWb_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubbsat_dv)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vw_vsub_VwVwQ_carry __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubcarry)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vub_vsub_VubVb_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubububb_sat)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Vuw_vsub_VuwVuw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubuwsat)
#endif

#if __HVX_ARCH__ >= 62







#define Q6_Wuw_vsub_WuwWuw_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsubuwsat_dv)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vb_vabs_Vb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsb)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vb_vabs_Vb_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vabsb_sat)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vh_vaslacc_VhVhR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaslh_acc)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vh_vasracc_VhVhR __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasrh_acc)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vub_vasr_VuhVuhR_rnd_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasruhubrndsat)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vub_vasr_VuhVuhR_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasruhubsat)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vuh_vasr_VuwVuwR_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasruwuhsat)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vb_vavg_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgb)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vb_vavg_VbVb_rnd __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavgbrnd)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vuw_vavg_VuwVuw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavguw)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vuw_vavg_VuwVuw_rnd __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vavguwrnd)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_W_vzero __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vdd0)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vgather_ARMVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgathermh)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vgather_AQRMVh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgathermhq)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vgather_ARMWw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgathermhw)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vgather_AQRMWw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgathermhwq)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vgather_ARMVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgathermw)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vgather_AQRMVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vgathermwq)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vh_vlut4_VuhPh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vlut4)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Wh_vmpa_WubRub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpabuu)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Wh_vmpaacc_WhWubRub __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpabuu_acc)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vh_vmpa_VhVhVhPh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpahhsat)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vh_vmpa_VhVhVuhPuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpauhuhsat)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vh_vmps_VhVhVuhPuh_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpsuhuhsat)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Ww_vmpyacc_WwVhRh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyh_acc)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vuw_vmpye_VuhRuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyuhe)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vuw_vmpyeacc_VuwVuhRuh __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vmpyuhe_acc)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vb_vnavg_VbVb __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vnavgb)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vb_prefixsum_Q __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vprefixqb)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vh_prefixsum_Q __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vprefixqh)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_Vw_prefixsum_Q __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vprefixqw)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vscatter_RMVhV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vscattermh)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vscatteracc_RMVhV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vscattermh_add)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vscatter_QRMVhV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vscattermhq)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vscatter_RMWwV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vscattermhw)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vscatteracc_RMWwV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vscattermhw_add)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vscatter_QRMWwV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vscattermhwq)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vscatter_RMVwV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vscattermw)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vscatteracc_RMVwV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vscattermw_add)
#endif

#if __HVX_ARCH__ >= 65







#define Q6_vscatter_QRMVwV __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vscattermwq)
#endif

#if __HVX_ARCH__ >= 66







#define Q6_Vw_vadd_VwVwQ_carry_sat __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vaddcarrysat)
#endif

#if __HVX_ARCH__ >= 66







#define Q6_Ww_vasrinto_WwVwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vasr_into)
#endif

#if __HVX_ARCH__ >= 66







#define Q6_Vuw_vrotr_VuwVuw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vrotr)
#endif

#if __HVX_ARCH__ >= 66







#define Q6_Vw_vsatdw_VwVw __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_vsatdw)
#endif

#if __HVX_ARCH__ >= 68







#define Q6_Ww_v6mpy_WubWbI_h __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_v6mpyhubs10)
#endif

#if __HVX_ARCH__ >= 68







#define Q6_Ww_v6mpyacc_WwWubWbI_h __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_v6mpyhubs10_vxx)
#endif

#if __HVX_ARCH__ >= 68







#define Q6_Ww_v6mpy_WubWbI_v __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_v6mpyvubs10)
#endif

#if __HVX_ARCH__ >= 68







#define Q6_Ww_v6mpyacc_WwWubWbI_v __BUILTIN_VECTOR_WRAP(__builtin_HEXAGON_V6_v6mpyvubs10_vxx)
#endif

#endif

#endif
