
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <sys/types.h>

#include "SescConf.h"
#include "ThermTrace.h"

void ThermTrace::tokenize(const char *str, TokenVectorType &tokens) {

  I(tokens.empty());

  const char *new_pos;
  do {
    if (str[0] == ' ' || str[0] == '\t' || str[0] == '\n') {
      str++;
      continue;
    }

    new_pos = strchr(str, ' ');
    if (new_pos == 0)
      new_pos = strchr(str, '\t');
    else {
      const char *tab_pos = strchr(str, '\t');
      if (tab_pos && tab_pos < new_pos)
	new_pos = tab_pos;
    }
    if (new_pos == 0)
      new_pos = strchr(str, '\n');
    else {
      const char *ret_pos = strchr(str, '\n');
      if (ret_pos && ret_pos < new_pos)
	new_pos = ret_pos;
    }

    if (new_pos == 0) {
      // last string
      tokens.push_back(strdup(str));
      break;
    }

    int32_t len = new_pos-str;
    char *new_str = (char *)malloc(len+4);
    strncpy(new_str,str,len);
    new_str[len]=0;
    tokens.push_back(new_str);

    str = new_pos;

  }while(str[0] != 0);
}

void ThermTrace::read_sesc_variable(const char *input_file) {

}

bool ThermTrace::grep(const char *line, const char *pattern) {
  return true;
}

const ThermTrace::FLPUnit *ThermTrace::findBlock(const char *name) const {

  for(size_t id=0;id<flp.size();id++) {
    if (strcasecmp(name, flp[id]->name) == 0)
      return flp[id];
  }

  return 0; // Not found
}

void ThermTrace::read_floorplan_mapping() {

  GI(input_file_[0]!=0,!mapping.empty()); // first call read_sesc_variable

  const char *flpSec = SescConf->getCharPtr("","floorplan");
  size_t min = SescConf->getRecordMin(flpSec,"blockDescr");
  size_t max = SescConf->getRecordMax(flpSec,"blockDescr");

  // Floor plan parameters
  for(size_t id=min;id<=max;id++) {
    if (!SescConf->checkCharPtr(flpSec,"blockDescr", id)) {
      MSG("There is a WHOLE on the floorplan. This can create problems blockDescr[%d]", id);
      exit(-1);
      continue;
    }

    const char *blockDescr = SescConf->getCharPtr(flpSec,"blockDescr", id);
    TokenVectorType descr;
    tokenize(blockDescr, descr);

    FLPUnit *xflp = new FLPUnit(strdup(descr[0]));
    xflp->id   = id;
    xflp->area = atof(descr[1])*atof(descr[2]);

    xflp->x       = atof(descr[3]);
    xflp->y       = atof(descr[4]);
    xflp->delta_x = atof(descr[1]);
    xflp->delta_y = atof(descr[2]);

    const char *blockMatch = SescConf->getCharPtr(flpSec,"blockMatch", id);
    tokenize(blockMatch, xflp->match);

    flp.push_back(xflp);
  }

  // Find mappings between variables and flp

  for(size_t id=0;id<flp.size();id++) {
    for(size_t i=0; i<flp[id]->match.size(); i++) {
      for(size_t j=0; j<mapping.size(); j++) {
	if (grep(mapping[j].name, flp[id]->match[i])) {

#ifdef DEBUG
	  MSG("mapping[%d].map[%d]=%d (%s -> %s)", 
	      j, mapping[j].map.size(), id, flp[id]->match[i], mapping[j].name);
#endif

	  I(id < flp.size());
	  flp[id]->units++;
	  I(j < mapping.size());
	  mapping[j].area += flp[id]->area;
	  mapping[j].map.push_back(id);
	}
      }
    }
  }

  for(size_t i=0;i<mapping.size();i++) {
    for(size_t j=0; j<mapping[i].map.size(); j++) {
      float ratio = flp[mapping[i].map[j]]->area/mapping[i].area;
      mapping[i].ratio.push_back(ratio);
    }
  }

  for(size_t j=0; j<mapping.size(); j++) {
    I(mapping[j].map.size() == mapping[j].ratio.size());
    if (mapping[j].map.empty()) {
      MSG("Error: sesc variable %s [%d] does not update any block", mapping[j].name, j);
      exit(3);
    }
  }
}

ThermTrace::ThermTrace(const char *input_file)
  : input_file_(strdup(input_file?input_file:"")) {
	
  if( input_file) {
    read_sesc_variable(input_file);
  }
    
  read_floorplan_mapping();
}

void ThermTrace::dump() const {

  for(size_t i=0;i<flp.size();i++) {
    flp[i]->dump();
  }

  for(size_t i=0;i<mapping.size();i++) {
    mapping[i].dump();
  }
  
}

bool ThermTrace::read_energy() {

  return true;
}
