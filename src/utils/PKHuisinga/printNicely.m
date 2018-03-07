function out = printNicely(data)

organName = ['lun';'art';'bra';'adi';'hea';'kid';'mus';'bon';'ski';...
             'gut';'spl';'liv';'ven'];

fprintf('\n\n');
for m=1:13
  fprintf('  %s = %3.4f\n',organName(m,:),data(m))  
end;
fprintf('\n\n');
