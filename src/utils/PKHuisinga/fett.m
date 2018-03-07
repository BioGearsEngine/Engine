function out = fett(figNr)

% Increases line width, font size, etc.
% Call by, e.g., fett(2)

figure(figNr);

lw = 2;   %line width
fs = 18;  %fontsize

h = findobj(gca,'Type','line');
set(h,'LineWidth',lw);

set(gca,'FontSize',fs);
set(gca,'LineWidth',lw);
set(get(gca,'title'),'Fontsize',fs);
set(get(gca,'xlabel'),'Fontsize',fs);
set(get(gca,'ylabel'),'Fontsize',fs);
set(legend,'Fontsize',fs); 