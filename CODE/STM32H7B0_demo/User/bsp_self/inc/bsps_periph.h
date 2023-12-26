/*
 * @Author: SUN  BI4NEG@gmail.com
 * @Date: 2023-11-12 15:13:52
 * @LastEditTime: 2023-12-26 17:47:35
 * @Description: ����д���
 */
#ifndef __BSPS_PERIPH_H
#define __BSPS_PERIPH_H


#define OSC_UNIT_NS 0
#define OSC_UNIT_US 1
#define OSC_UNIT_MS 2
#define OSC_UNIT_S 3

//ʱ�������ṹ�嶨��
typedef struct
{
    //ʱ����ʾ�ַ�
    char *str;
    //ʱ��
    float osc_time;
    //ʱ����λ
    unsigned int osc_unit;
    //�����ӳ�ʱ��
    unsigned int osc_trig_delay;

} osc_time_t;

//��ֱ��ѹ����ṹ�嶨��
typedef struct
{
    //x1������ʾ�ַ�
	char * str;
    //x10������ʾ�ַ�
	char * strX10;
    //�̵���˥����·ѡ��
	unsigned short rly_att;
    //��ֱ��ѹ������ֵ��ʾ
    unsigned short mv_int;
    //����˥����·ѡ��
	unsigned short att_sel[2];
    //ʵ�����뵽ADC�����汶����ע���ֵδ����10��˥��
    float gain;
}osc_vol_scale_t;

extern const osc_time_t osc_tim_table[];
extern osc_vol_scale_t osc_vol_scale_table[];

void bsps_sa_rly_att_ch1(void);
void bsps_sa_rly_no_att_ch1(void);
void bsps_sa_rly_att_ch2(void);
void bsps_sa_rly_no_att_ch2(void);

void bsps_sa_att_sel_ch1(uint8_t att);
void bsps_sa_att_sel_ch2(uint8_t att);

void bsps_sa_oh_dc_ch1(void);
void bsps_sa_oh_ac_ch1(void);
void bsps_sa_oh_dc_ch2(void);
void bsps_sa_oh_ac_ch2(void);

void bsps_sa_sw_only_ch1(void);
void bsps_sa_sw_only_ch2(void);
void bsps_sa_sw_ch1_ch2(void);
void bsps_sa_sw_ch2_ch1(void);

void bsps_sa_set_trig_vol(float vol);

void bsps_sa_adc_mode_standby(void);
void bsps_sa_adc_mode_only_ch1(void);
void bsps_sa_adc_mode_ch1_ch2_no_align(void);
void bsps_sa_adc_mode_ch1_ch2_align(void);

void bsps_sa_set_pwm_pulse_ch1(uint16_t pulse);
void bsps_sa_set_pwm_pulse_ch2(uint16_t pulse);

void bsps_time_psc_set(uint8_t index);
void bsps_time_inc(void);
void bsps_time_dec(void);

void bsps_vol_scale_set_ch1(uint8_t index);
void bsps_vol_scale_inc_ch1(void);
void bsps_vol_scale_dec_ch1(void);

void bsps_vol_scale_set_ch2(uint8_t index);
void bsps_vol_scale_inc_ch2(void);
void bsps_vol_scale_dec_ch2(void);

void bsps_trig_lev_set(uint16_t trig);
void bsps_ratio_set(uint8_t ratio);

void bsps_beep_on(uint16_t time);

#endif
