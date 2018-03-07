function [doxyFormattedTable] = doxyTable(inputTable,tableName)

pathBase = pwd;
%% Get info about the inputTable into usable form
rows = inputTable.Properties.RowNames;
columns = inputTable.Properties.VariableNames;
cellArray = table2cell(inputTable);

[CAlength, CAwidth] = size(cellArray);

if(~isempty(columns))      %make variable names first row if they exist
    for i = CAlength+1:-1:2
        cellArray(i,:) = cellArray(i-1,:);
    end
    cellArray(1,:) = columns;
end
[con loc] = ismember('PercentError', columns);

if(con ~=0)
    for i = 2:1:CAlength+1
        if(~isnan(str2double(cellArray{i,loc})))
            toHTML = str2double(cellArray{i,loc});
            if(toHTML <= 10)
                cellArray{i,loc} = ['<span class="success">', char(num2str(toHTML,'%-5.2g')),'</span>'];
            elseif(toHTML > 10 && toHTML <= 30)
                cellArray{i,loc} = ['<span class="warning">', char(num2str(toHTML,'%-5.2g')),'</span>'];
            else
                cellArray{i,loc} = ['<span class="danger">', char(num2str(toHTML,'%-5.2g')),'</span>'];
            end
        elseif(ischar(cellArray{i,loc}))
            if(strcmp('Within range',cellArray{i,loc}))
                cellArray{i,loc} = ['<span class="success">',cellArray{i,loc},'</span>'];
            else
                cellArray{i,loc} = ['<span class="danger">',cellArray{i,loc},'</span>'];
            end
        end
    end
end
if(~isempty(rows))      %make rownames first column if they exist
    for i = CAwidth+1:-1:2
        cellArray(:,i) = cellArray(:,i-1);
    end
    cellArray(1,1) = {'Parameter Name'};
    cellArray(2:end,1) = rows;
end

[CAlength, CAwidth] = size(cellArray); %refresh the size measurement

%% Looks like 2nd row needs to be |----------------                     | or something like that... so adding that in
% |------------------------  => string to add into 2nd row
for i = CAlength+1:-1:2
    cellArray(i,:) = cellArray(i-1,:);
end

cellArray(2,:) = {'|------------------------'};

[CAlength, CAwidth] = size(cellArray);
%% Now make all cells characters, the same length per row, and add the |

charCellArray = cell(CAlength,CAwidth);

for i = 1:1:CAlength
    for j = 1:1:CAwidth
        if(isnumeric(cellArray{i,j}))
            charCellArray{i,j} = char(num2str(cellArray{i,j},'%-5.2g'));
        else
            charCellArray{i,j} = char(cellArray{i,j});
        end
    end
end

%first add the '|' to every row except the 2ns

for i = 1:1:CAlength
    if(i == 2)
        continue;
    end
    for j = 1:1:CAwidth
       charArray = charCellArray{i,j};
       for k = length(charArray)+1:-1:2
           charArray(k) = charArray(k-1);
       end
       charArray(1) = char('|');
       charCellArray{i,j} = charArray;
    end
end

% make the same length
rowSize = 0;
for i = 1:1:CAwidth
    for j = 1:1:CAlength
        if(length(charCellArray{j,i})>rowSize)
            rowSize = length(charCellArray{j,i});
        end
    end
    for j = 1:1:CAlength
        charArray = charCellArray{j,i};
        while (length(charArray)<(rowSize + 5))           
            charArray(end + 1) = ' ';
        end
        charCellArray{j,i} = charArray;
    end
end
%% add a final '|' to the last column

for i = 1:1:CAlength
    charArray = charCellArray{i,end};
    charArray(end+1) = char('|');
    charCellArray{i,end}= charArray;
end

doxyFormattedTable = charCellArray;
%% save it all
formatSpec = '%s ';
[DFTlength, DFTwidth] = size(doxyFormattedTable);
for i = 2:1:DFTwidth-1
    formatSpec(end+1:end+3) = '%s ';
    if(i==DFTwidth-1)
        formatSpec(end+1:end+4) = '%s\n';
    end
end
%savePath = [pathBase, '\..\..\..\docs\Validation\MatlabTables\', char(tableName), 'doxyTable.txt'];
savePath = [pathBase,'\',char(tableName), 'doxyTable.txt'];
fID = fopen(savePath, 'w');
for i = 1:1:DFTlength
    fprintf(fID,formatSpec,doxyFormattedTable{i,:});
end