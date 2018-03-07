function [holder] = PKDataShift(DAT);




m = max(DAT(:,2));


loc = find(DAT(:,2) == m);




[L W] = size(DAT);
counter = 1;

for i = 1:L
   if i >= loc
      holder(counter,:) = DAT(i,:);
      counter = counter + 1;
   end
end