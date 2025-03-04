/*
Portions Copyright 2019-2021 ZomboDB, LLC.
Portions Copyright 2021-2022 Technology Concepts & Design, Inc. <support@tcdi.com>

All rights reserved.

Use of this source code is governed by the MIT license that can be found in the LICENSE file.
*/
#include "postgres.h"
#include "pg_config.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "pgstat.h"

#include "access/amapi.h"
#include "access/genam.h"
#include "access/gin.h"
#include "access/gist.h"
#include "access/heapam.h"
#include "access/htup.h"
#include "access/htup_details.h"
#include "access/reloptions.h"
#include "access/relscan.h"
#include "access/skey.h"
#include "access/sysattr.h"
#include "access/xact.h"
#include "catalog/dependency.h"
#include "catalog/index.h"
#include "catalog/namespace.h"
#include "catalog/objectaddress.h"
#include "catalog/pg_class.h"
#include "catalog/pg_enum.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_proc.h"
#include "catalog/pg_trigger.h"
#include "catalog/pg_type.h"
#include "commands/comment.h"
#include "commands/dbcommands.h"
#include "commands/defrem.h"
#include "commands/event_trigger.h"
#include "commands/explain.h"
#include "commands/proclang.h"
#include "commands/tablecmds.h"
#include "commands/trigger.h"
#include "commands/vacuum.h"
#include "executor/executor.h"
#include "executor/spi.h"
#include "foreign/fdwapi.h"
#include "foreign/foreign.h"
#include "mb/pg_wchar.h"

#define ScanKey struct ScanKeyData *
#include "nodes/execnodes.h"
#undef ScanKey

#include "nodes/extensible.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "nodes/nodes.h"
#include "nodes/print.h"
#include "nodes/relation.h"
#include "nodes/replnodes.h"
#include "nodes/tidbitmap.h"
#include "nodes/value.h"
#include "optimizer/clauses.h"
#include "optimizer/cost.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"
#include "optimizer/planmain.h"
#include "optimizer/planner.h"
#include "optimizer/restrictinfo.h"
#include "optimizer/tlist.h"
#include "parser/parse_func.h"
#include "parser/parse_type.h"
#include "parser/parser.h"
#include "parser/parsetree.h"
#include "postmaster/bgworker.h"
#include "replication/output_plugin.h"
#include "rewrite/rewriteHandler.h"
#include "storage/block.h"
#include "storage/bufmgr.h"
#include "storage/buffile.h"
#include "storage/ipc.h"
#include "storage/itemptr.h"
#include "storage/lwlock.h"
#include "storage/procarray.h"
#include "tcop/tcopprot.h"
#include "tcop/utility.h"
#include "tsearch/ts_public.h"
#include "tsearch/ts_utils.h"
#include "utils/builtins.h"
#include "utils/date.h"
#include "utils/datetime.h"

#define double float8
#include "utils/geo_decls.h"
#undef double

#include "utils/guc.h"
#include "utils/json.h"
#include "utils/jsonb.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/palloc.h"
#include "utils/rel.h"
#include "utils/relcache.h"
#include "utils/sampling.h"
#include "utils/selfuncs.h"
#include "utils/snapmgr.h"
#include "utils/syscache.h"
#include "utils/typcache.h"
