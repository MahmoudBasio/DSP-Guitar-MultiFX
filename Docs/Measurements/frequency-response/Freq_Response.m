%% DSP Guitar MultiFX — frequency-response characterization
% Experimental firmware: earlier effects-off sketch with the 3.2 kHz
% pre-filter enabled. NOT the current firmware's unfiltered dry route.
% Input/output readings in mVpp. Approximate 10 kHz Vout (75 mVpp).
% Measurements are single readings; no uncertainty estimate is available.
% Requires Signal Processing Toolbox for butter() and freqz().
clc; clear; close all;

frequency_Hz = [20 50 100 200 500 1000 2000 5000 10000 15000];
input_mVpp   = [555 555 558 560 568 580 608 630 634 640];
output_mVpp  = [584 645 656 660 660 664 640 260 75 40];
gain_dB = 20*log10(output_mVpp ./ input_mVpp);

results = table(frequency_Hz',input_mVpp',output_mVpp',gain_dB', ...
    'VariableNames',{'Frequency_Hz','Input_mVpp','Output_mVpp','Gain_dB'});
disp(results);
writetable(results, 'frequency_response.csv');

figWidth = 3.5; figHeight = 2.6;
majorTicks = [20 100 1000 10000 20000];
tickLabels = {'20','100','1k','10k','20k'};

%% Figure 1 — absolute system gain
fig1 = figure('Color','w','Units','inches', ...
    'Position',[2 2 figWidth figHeight]);
semilogx(frequency_Hz,gain_dB,'-ok','LineWidth',1.2, ...
    'MarkerSize',4.5,'MarkerFaceColor','k');
xlabel('Frequency (Hz)'); ylabel('Voltage Gain (dB)');
xlim([20 20000]); ylim([-30 5]);
setIEEEAxes(gca,majorTicks,tickLabels);
exportgraphics(fig1,'DSP_Guitar_Frequency_Response.pdf', ...
    'ContentType','vector');
exportgraphics(fig1,'DSP_Guitar_Frequency_Response.png','Resolution',600);

%% Figure 2 — measured vs approximate 3.2 kHz filter
fs = 44100; fc = 3200;
[b,a] = butter(2,fc/(fs/2),'low');
[H,f] = freqz(b,a,4096,fs);
theory_dB = 20*log10(max(abs(H),1e-12));

% Normalize each response to its gain at 1 kHz.
measuredRef = gain_dB(frequency_Hz == 1000);
measuredNormalized = gain_dB - measuredRef;
theoryRef = interp1(f,theory_dB,1000,'linear');
theoryNormalized = theory_dB - theoryRef;

fig2 = figure('Color','w','Units','inches', ...
    'Position',[2 2 figWidth figHeight]);
semilogx(frequency_Hz,measuredNormalized,'ok', ...
    'MarkerSize',4.5,'MarkerFaceColor','k');
hold on;
semilogx(f,theoryNormalized,'--','Color',[0.35 0.35 0.35], ...
    'LineWidth',1.2);
xlabel('Frequency (Hz)'); ylabel('Relative Gain (dB)');
xlim([20 20000]); ylim([-35 5]);
setIEEEAxes(gca,majorTicks,tickLabels);
legend('Measured system','Approximate 3.2 kHz LPF', ...
    'Location','southwest','FontSize',7);
exportgraphics(fig2,'Measured_vs_Theoretical.pdf', ...
    'ContentType','vector');
exportgraphics(fig2,'Measured_vs_Theoretical.png','Resolution',600);
disp('Exported CSV, vector PDFs and 600 DPI PNGs.');

function setIEEEAxes(ax,ticks,labels)
    ax.XScale = 'log';
    ax.XTick = ticks;
    ax.XTickLabel = labels;
    ax.XMinorTick = 'off'; ax.YMinorTick = 'off';
    ax.XGrid = 'on'; ax.YGrid = 'on';
    ax.XMinorGrid = 'off'; ax.YMinorGrid = 'off';
    ax.GridLineStyle = ':'; ax.GridAlpha = 0.25;
    ax.FontName = 'Times New Roman'; ax.FontSize = 9;
    ax.LineWidth = 0.8;
    box(ax,'on');
end
