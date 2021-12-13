clc
close all
clear variables

fontsize_axis = 20;
fontsize_ticks = 18;
fontsize_legend = 15;
fontsize_title = 20;
fontsize_linewidth = 3;
fontsize_marker = 8;

%% 
R5 = 10e3;
NTC_table = readtable('ntc.csv', 'HeaderLines',0);
NTC_Data = NTC_table{:,:};
temp = NTC_Data(:,1);
NTC_res = NTC_Data(:,2)*1e3;

V_ADC = 3.3.*NTC_res./(R5+NTC_res);
ADC_val = 2^10/3.3* V_ADC;
AD_VECT = 0:1023;
p = polyfit(ADC_val, temp,10);
poly_temp = round(polyval(p,AD_VECT),1);

figure('Name','ADC','units','normalized','outerposition',[0 0 0.4 0.4]);

plot(AD_VECT, poly_temp,"-r")
hold on
plot(ADC_val,temp, "Xb") 
title("vstupní hodnoty NTC pro 10bit ADC","FontSize",fontsize_title)
xlabel("ADC value - 10 bit resolution","FontSize",fontsize_axis)
ylabel("temperature [°C]","FontSize",fontsize_axis)
grid on
grid minor
saveas(gcf,"ADCplot","jpg")

dlmwrite("ADC_data.dlm", poly_temp*10, ',')