%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   importraw
%  
%    Created on: jan 29, 2014
%        Author: trs
%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function ret = importraw( varargin )

if(isempty(varargin))
     [filename, pathname] = uigetfile({'*.bin;*.csv'},'Select the bin or csv file');;
    varargin = {filename};
    if(varargin{1}==0)
        return;
    end
end

bin = strfind(varargin{1},'.bin');
cvs = strfind(varargin{1},'_varnames.csv');
if(~isempty(bin))
    fn = varargin{1}(1:bin-1);%load csv
end

if(~isempty(cvs))
    fn = varargin{1}(1:cvs-1);
end

%load config
cfn = [pathname fn '_varnames.csv'];
bfn = [pathname fn '.bin'];

if(isempty(dir(cfn)))
    disp([ 'csv file not found ' cfn]);
    return ;
end

if(isempty(dir(bfn)))
    disp([ 'bin file not found ' bfn]);
    return ;
end

disp(['Opening file' bfn]);
configfd = fopen(cfn);
type= fscanf(configfd,'%c,',1);
prec= fscanf(configfd,'%d,',1);
autosave_period = fscanf(configfd,'%d,',1);
variables = fscanf(configfd,'%d,',1);
varnames = fscanf(configfd,'%s');
varnames = regexp(varnames,',','split');%strsplit(varnames,',') ;

bdir = dir(bfn);
corrupted = mod(bdir.bytes,(variables*autosave_period*prec));

if (corrupted > 0)
    disp(['There are ' num2str(corrupted) ' bytes at the end of the file ']);
end

varl = (bdir.bytes-corrupted)/(variables*prec);
data = zeros((bdir.bytes-corrupted)/(variables*prec),variables);


if(type=='d')
    type = 'double';
    disp('Double Values');
elseif(type == 'f')
    type='float';
    disp('Float Values');
elseif(type == 'i')
    type = 'int32';
end

fid = fopen(bfn);
for i = 1:autosave_period:varl
    tmp = fread(fid,[autosave_period variables],type);
    data(i:i+autosave_period-1,:) = tmp;
    % tmp = fread(fid,[autosave_period variables],type)
end
ret.data = data;
ret.names=varnames;

