clc;
clear all;
close all;


DateTime = {'2007-01-01 00:00';'2007-02-01 00:00';'2007-03-01 00:00'};
Headers = {'Datetime','Data'};
Dat = [100,200,300];


 % // In the way you used fprintf it expects just strings ('%s\t%s\n'), 
 % // therefore Data should be composed exclusively by them.
 % // Numbers are converted to strings by using num2str
 % // by using cellfun we iteratively convert every element of num2cell(Dat')
 % // in strings, obtaining a cell
 Data = [DateTime,cellfun(@num2str, num2cell(Dat'), 'UniformOutput' , false )]; 
 Final = [Headers;Data];

 fid = fopen('test.txt','wt');

 % // this iterates fprintf on the cell rows, giving you the output
 cellfun(@(x,y) fprintf(fid,'%s\t%s\n',x,y),Final(:,1),Final(:,2));       
 fclose(fid);
 
%  for i = 1 : size(Final,1)
%     fprintf(fid,'%s ', Final{i,:} );
%     fprintf(fid,'\n');
% end




%%



%A = [mean_acc variance_acc std_acc median_acc min_acc max_acc rms_acc corr_acc SMA_acc pwrTot_acc pwrTotdiff_acc Sum_of_ZC Sum_of_Energy ...
%     SVM_acc e median_acc_freq Info_Entropy Power_from_PSD dc_acc key_coeff_acc];

% mat1 = [mean_acc;variance_acc;std_acc;median_acc;min_acc;max_acc;rms_acc;corr_acc];
% mat2 = [pwrTot_acc;pwrTotdiff_acc;median_acc_freq;Info_Entropy;Power_from_PSD;key_coeff_acc];
% mat3 = [SMA_acc;Sum_of_ZC;Sum_of_Energy;SVM_acc;e;dc_acc];
% fName = 'DR.NATESAN_DRILL_1.txt';
% fid = fopen(fName,'w');
% 
% fprintf(fid,'%s \t %s \t %s \t %s \t %s \t %s \t %s \t %s \n','Mean','Variance','Std-Deviation','Median',...
%       'Minimum','Maximum', 'RMS','Corrcoeff');
% fprintf(fid,'%f %f %f %f %f %f %f %f \n', mat1);
% fprintf(fid,'%s \t %s \t %s \t %s \t %s \t %s \t %s \t %s \n', 'Power of signal', 'Sample Differences Power','median_acc_freq', 'Info_Entropy', 'Power_from_PSD',...
%       'key_coefficient');
% fprintf(fid,'%f %f %f %f %f %f %f %f \n', mat2);
% fclose(fid); 
% %fprintf(fileID,'%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n','Mean','Variance','Std-Deviation','Median',...
% %     'Minimum','Maximum', 'RMS','Corrcoeff', 'Signal Magnitude area', 'Power of signal', 'Sample Differences Power', 'Sum of zero-crossing',...
% %     'Sum of Energy', 'Signal Vector Magnitude', 'Entropy','median_acc_freq', 'Info_Entropy', 'Power_from_PSD','dc_Component', 'key_coefficient');
% % fprintf(fileID,'/t %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n', mean_acc);
% % 
% % 
% % 
% % fclose(fileID);
% % 
% str = 'DR.NATESAN_DRILL_1';      %# A string
% mat1 = [mean_acc;variance_acc;std_acc;median_acc;min_acc;max_acc;rms_acc;corr_acc;pwrTot_acc;pwrTotdiff_acc;median_acc_freq;Info_Entropy;Power_from_PSD;key_coeff_acc];            
% mat2 = [SMA_acc;Sum_of_ZC;Sum_of_Energy;SVM_acc;e;dc_acc];
% fName = 'DR.NATESAN_DRILL_1.txt';  %# A file name
% fid = fopen(fName,'w');            %# Open the file
% if fid ~= -1
%   fprintf(fid,'%s\r\n',str);       %# Print the string
%   fprintf(fid,'%s \t ','Mean');
%   dlmwrite(fName,mean_acc,'-append',...  %# Print the matrix
%          'delimiter','\t',...
%          'newline','pc');
%   fclose(fid);                     %# Close the file
% ends