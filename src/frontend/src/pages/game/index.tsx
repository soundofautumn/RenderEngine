import * as React from 'react';

import { backend_endpoint, engine_name, game_engine_fps as engine_fps, client, retry_max_times } from '../../utils/client';
import { drawFunctions } from '../../utils/drawFuncs';

import './index.css'

type ControlKey = 'ArrowUp' | 'ArrowDown' | 'ArrowLeft' | 'ArrowRight' | 'Space';
type Direction = 'up' | 'down' | 'left' | 'right' | undefined;

interface IChess {
  type: 'empty' | 'wire' | 'router';
  owner?: number;
  primitive?: number[];
}

const OwnerColors = [
  '#2ecc71',
  '#e67e22',
  '#e74c3c',
  '#3498db',
  '#f1c40f',
  '#9b59b6',
  '#6c5ce7',
  '#e84393',
  '#2d3436',
  '#fdcb6e',
  '#0984e3',
  '#e17055',
].sort(() => Math.random() - 0.5);

function DeletePrimitive(index: number) {
  return client("/engine/primitive/modify", {
    data: {
      Primitive: {
        Line: {
          p1: {
            x: 0,
            y: 0,
          },
          p2: {
            x: 0,
            y: 0,
          },
          algorithm: 0,
        }
      },
      Index: index,
    }
  })
}

function ChangeColor(color: string, back_color = '#000000', fore_alpha = 255, back_alpha = 0): Promise<number> {
  return new Promise<number>((resolve, reject) => {
    client('/engine/primitive/push_back', {
      data: {
        PenOptions: {
          color: {
            r: parseInt(color.slice(1, 3), 16),
            g: parseInt(color.slice(3, 5), 16),
            b: parseInt(color.slice(5, 7), 16),
            a: fore_alpha,
          },
          width: 1,
          type: 0,
          dash: 1,
          fill_color: {
            r: parseInt(back_color.slice(1, 3), 16),
            g: parseInt(back_color.slice(3, 5), 16),
            b: parseInt(back_color.slice(5, 7), 16),
            a: back_alpha,
          },
        },
      }
    }).then(r => {
      resolve(r.data.data.index);
    }).catch(reject);
  })
}

async function DrawSelectBox(x: number, y: number, owner: number): Promise<number[]> {
  const color = OwnerColors[owner] || '#ffffff';
  const primitives: number[] = [
    await ChangeColor(color, '#000000', 200),
    ...await Promise.all([
      drawFunctions.Circle.draw({
        pointers: [
          { x, y },
          { x, y: y + 46 }
        ]
      }),
      drawFunctions.Circle.draw({
        pointers: [
          { x, y },
          { x, y: y + 47 }
        ]
      }),
      drawFunctions.Circle.draw({
        pointers: [
          { x, y },
          { x, y: y + 48 }
        ]
      }),
      drawFunctions.Circle.draw({
        pointers: [
          { x, y },
          { x, y: y + 49 }
        ]
      }),
    ]),
  ];
  return primitives;
}

async function DrawRouter(x: number, y: number, owner: number, alpha = 80): Promise<number[]> {
  const color = OwnerColors[owner] || '#ffffff';
  const primitives: number[] = [
    await ChangeColor('#7f8c8d', '#7f8c8d', 255, 255),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 45 }
      ]
    }),
    await drawFunctions.Seed.draw({
      pointers: [
        { x, y },
      ]
    }),

    await ChangeColor(color, color, 255, 40),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 42 }
      ]
    }),
    await drawFunctions.Seed.draw({
      pointers: [
        { x, y },
      ]
    }),

    await ChangeColor('#000000', color, 255, alpha),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 32 }
      ]
    }),
    await drawFunctions.Seed.draw({
      pointers: [
        { x, y },
      ]
    }),

    await ChangeColor(color, '#000000', 40),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 32 }
      ]
    }),

    await ChangeColor('#7f8c8d', '#000000', 0, 255),
    ...await Promise.all([
      drawFunctions.Rectangle.draw({
        pointers: [
          { x: x - 5, y: y - 35 },
          { x: x + 5, y: y + 35 }
        ]
      }),
      drawFunctions.Rectangle.draw({
        pointers: [
          { x: x - 35, y: y - 5 },
          { x: x + 35, y: y + 5 }
        ]
      }),
    ]),

    await ChangeColor(color, color, 0, 40),
    ...await Promise.all([
      drawFunctions.Rectangle.draw({
        pointers: [
          { x: x - 5, y: y - 35 },
          { x: x + 5, y: y + 35 }
        ]
      }),
      drawFunctions.Rectangle.draw({
        pointers: [
          { x: x - 35, y: y - 5 },
          { x: x + 35, y: y + 5 }
        ]
      }),
    ]),


    await ChangeColor('#7f8c8d', '#000000', 255, 255),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 12 }
      ]
    }),
    await drawFunctions.Seed.draw({
      pointers: [
        { x, y },
      ]
    }),

    await ChangeColor(color, color, 255, 40),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 12 }
      ]
    }),
    await drawFunctions.Seed.draw({
      pointers: [
        { x, y },
      ]
    }),

    await ChangeColor('#000000', color, 255, 40),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 12 }
      ]
    }),
    await ChangeColor(color, color, 40, 40),
    await drawFunctions.Circle.draw({
      pointers: [
        { x, y },
        { x, y: y + 12 }
      ]
    }),

    await ChangeColor(color, color, 255, 0),
    ...await Promise.all([
      drawFunctions.Line.draw({
        pointers: [
          { x: x - 3, y: y - 3 },
          { x: x - 3, y: y - 3 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x, y: y - 3 },
          { x, y: y - 3 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x + 3, y: y - 3 },
          { x: x + 3, y: y - 3 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x - 3, y },
          { x: x - 3, y },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x, y },
          { x, y },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x + 3, y },
          { x: x + 3, y },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x - 3, y: y + 3 },
          { x: x - 3, y: y + 3 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x, y: y + 3 },
          { x, y: y + 3 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x + 3, y: y + 3 },
          { x: x + 3, y: y + 3 },
        ]
      }),

      drawFunctions.Line.draw({
        pointers: [
          { x: x, y: y + 10 },
          { x: x, y: y + 34 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x, y: y - 10 },
          { x: x, y: y - 34 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x - 10, y: y },
          { x: x - 34, y: y },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x + 10, y: y },
          { x: x + 34, y: y },
        ]
      }),

      drawFunctions.Line.draw({
        pointers: [
          { x: x - 2, y: y + 10 },
          { x: x + 2, y: y + 10 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x - 2, y: y + 34 },
          { x: x + 2, y: y + 34 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x - 2, y: y - 10 },
          { x: x + 2, y: y - 10 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x - 2, y: y - 34 },
          { x: x + 2, y: y - 34 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x + 10, y: y - 2 },
          { x: x + 10, y: y + 2 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x + 34, y: y - 2 },
          { x: x + 34, y: y + 2 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x - 10, y: y - 2 },
          { x: x - 10, y: y + 2 },
        ]
      }),
      drawFunctions.Line.draw({
        pointers: [
          { x: x - 34, y: y - 2 },
          { x: x - 34, y: y + 2 },
        ]
      }),
    ])
  ];
  return primitives;
}

function randomColor() {
  return '#' + Math.floor(Math.random() * 0xffffff).toString(16).padStart(6, '0');
}

// 随机生成不同颜色的从屏幕下方开始的贝塞尔曲线来模拟烟花
async function DrawWin() {
  for (let i = 0; i < 100; i++) {
    const primitives: number[] = [];
    primitives.push(await ChangeColor(randomColor(), randomColor(), 255, 255))
    primitives.push(await drawFunctions.Bezier.draw({
      pointers: [
        { x: Math.floor(Math.random() * 800), y: Math.floor(Math.random() * 800) },
        { x: Math.floor(Math.random() * 800), y: Math.floor(Math.random() * 800) },
        { x: Math.floor(Math.random() * 800), y: Math.floor(Math.random() * 800) },
        { x: Math.floor(Math.random() * 800), y: Math.floor(Math.random() * 800) },
      ]
    }))
    if (i % 5 == 0)
      await new Promise(r => setTimeout(r, 100));
    setTimeout(() => {
      primitives.forEach(primitive => {
        DeletePrimitive(primitive);
      })
    }, 100)
  }
  return;
}

async function PutWire(from: { x: number, y: number }, to: { x: number, y: number }, owner: number) {
  const color = OwnerColors[owner] || '#ffffff';
  const primitives: number[] = [
    await ChangeColor(color),
    await drawFunctions.Line.draw({
      pointers: [
        { x: from.x * 100 + 50, y: from.y * 100 + 50 },
        { x: to.x * 100 + 50, y: to.y * 100 + 50 }
      ]
    }),
  ];
  return primitives;
}

function GenerateChessboard(): IChess[][] {
  const emptyBoard: IChess[][] = Array.from({ length: 6 }, () => Array.from({ length: 8 }, () => ({ type: 'empty' })));
  let borderRouterCount = 0;
  for (let i = 0; i < Math.floor(3 + Math.random() * 2); i++)
    for (let j = 0; j < 2; j++) {
      let x, y;
      do {
        x = Math.floor(Math.random() * 8);
        y = Math.floor(Math.random() * 6);
      } while (emptyBoard[y][x].type !== 'empty' || (x === 0 || y === 0 || x === 7 || y === 5) && borderRouterCount >= 2);
      if (x === 0 || y === 0 || x === 7 || y === 5) borderRouterCount++;
      emptyBoard[y][x] = { type: 'router', owner: i };
    }
  return emptyBoard;
}

export default function Game() {
  const canvasRef = React.useRef<HTMLCanvasElement>(null);

  /* 处理键盘事件 */
  const [pressedKeys, setPressedKeys] = React.useState<ControlKey[]>([]);
  React.useEffect(() => {
    document.getElementById('pressed-key-list')!.scrollTo({
      left: document.getElementById('pressed-key-list')!.scrollWidth,
      behavior: 'smooth',
    });
  }, [pressedKeys])
  const handleKeyPressed = (e: React.KeyboardEvent<HTMLElement>) => {
    const extendKeyMap: { [key: string]: ControlKey } = {
      w: 'ArrowUp',
      a: 'ArrowLeft',
      s: 'ArrowDown',
      d: 'ArrowRight',
      ' ': 'Space',
    }
    if (e.key === 'ArrowUp' || e.key === 'ArrowDown' || e.key === 'ArrowLeft' || e.key === 'ArrowRight') {
      e.preventDefault();
      handleGameKey(e.key as ControlKey);
    }
    if (Object.keys(extendKeyMap).includes(e.key)) {
      e.preventDefault();
      handleGameKey(extendKeyMap[e.key]);
    }
  }
  const handleGameKey = (key: ControlKey) => {
    if (gameEnd.current) return;
    const directionMap: { [key in ControlKey]: Direction } = {
      'ArrowUp': 'up',
      'ArrowDown': 'down',
      'ArrowLeft': 'left',
      'ArrowRight': 'right',
      'Space': undefined,
    };
    setPressedKeys([...pressedKeys, key]);
    if (key === 'Space') handleSelect();
    else handleMove(directionMap[key]);
  }

  /* 连接后端 */
  const [, setLoading] = React.useState(true);
  const loadingRef = React.useRef(true);
  const fpsRef = React.useRef(0);
  const [fps, setFps] = React.useState(0);
  const first = React.useRef(true);
  const [start, setStart] = React.useState(true);
  const startTimeoutRef = React.useRef<number | null>(null);
  const [errorMessage, setErrorMessage] = React.useState('等待后端响应...');
  React.useEffect(() => {
    if (!first.current) return;
    first.current = false;

    const width = 800;
    const height = 600;

    startTimeoutRef.current = setTimeout(() => {
      setStart(false);
    }, 100);

    const ConnectToBackend = (retryTimes = 0) => new Promise((resolve, reject) => {
      client('/engine/create', {
        data: {
          name: engine_name,
          width,
          height,
        },
        timeout: 1000,
      }).then(r => {
        console.log(r.data);
        console.log(`Engine ${engine_name} created with ${width}x${height}`);
        setInterval(() => {
          setFps(fpsRef.current);
          if (fpsRef.current === 0) {
            window.location.reload();
          }
        }, 500);
        Init();
        resolve(1);
      }).catch(e => {
        console.error(e);
        setErrorMessage(`后端无响应，重新尝试连接.. ${retryTimes + 1}/${retry_max_times}`);
        if (retryTimes >= retry_max_times) {
          reject('Failed to create engine');
          return;
        } else {
          setTimeout(() => {
            ConnectToBackend(retryTimes + 1).then(resolve).catch(reject);
          }, 1000)
        }
      });
    });

    ConnectToBackend().then(() => {
      const ws = new WebSocket(`//${backend_endpoint}/engine/${engine_name}`);
      ws.onopen = function () {
        ws.send(`set_fps ${engine_fps}`);
        setLoading(false);
        if (startTimeoutRef.current) clearTimeout(startTimeoutRef.current);
        setStart(true);
        loadingRef.current = false;
      };
      ws.binaryType = "arraybuffer";
      ws.onmessage = function (event) {
        if (!canvasRef.current) return;

        fpsRef.current += 1;
        setTimeout(() => {
          fpsRef.current -= 1;
        }, 1000)

        const ctx = canvasRef.current.getContext("2d");
        canvasRef.current.width = width;
        canvasRef.current.height = height;
        const data = new Uint8ClampedArray(event.data);

        const imageData = new ImageData(data, width, height);
        if (ctx)
          ctx.putImageData(imageData, 0, 0);
      };
    }).catch(e => {
      console.error(e);
      setErrorMessage("后端无响应，请检查后端是否正常运行。");
    });
  }, [])

  /* 游戏逻辑 */
  const [chessboard, setChessboard] = React.useState<IChess[][]>([]);
  const [currentPosition, setCurrentPosition] = React.useState({ x: -1, y: -1 });
  const currentWiresPrimitive = React.useRef<number[]>([]);
  const allWiresPrimtives = React.useRef<{ [owner: number]: number[] }>({});
  const handleMove = (direction: Direction) => {
    const putWire = async (x: number, y: number) => {
      if (currentOwner < 0) {
        setCurrentPosition({ x, y });
        return;
      } else {
        if (chessboard[y][x].type === 'empty') {
          // 空的地方放线
          chessboard[y][x] = { type: 'wire', owner: currentOwner };
          currentWiresPrimitive.current.push(...await PutWire(currentPosition, { x, y }, currentOwner));
          setCurrentPosition({ x, y });
        }
        else if (chessboard[y][x].type === 'wire') {
          // 线不能交叉
          return;
        } else if (chessboard[y][x].type === 'router') {
          // 不是自己的路由器不能进
          if (chessboard[y][x].owner !== currentOwner || currentStartChess && y === currentStartChess.y && x === currentStartChess.x)
            return;
          else {
            // 成功连线
            currentWiresPrimitive.current.push(...await PutWire(currentPosition, { x, y }, currentOwner));
            allWiresPrimtives.current[currentOwner] = currentWiresPrimitive.current;
            currentWiresPrimitive.current = [];

            for (let cy = 0; cy < chessboard.length; cy++) {
              for (let cx = 0; cx < chessboard[cy].length; cx++) {
                const chess = chessboard[cy][cx];
                if (chess.type === 'router' && chess.owner === currentOwner) {
                  chess.primitive?.forEach(primitive => {
                    DeletePrimitive(primitive);
                  })
                  chess.primitive = await DrawRouter(cx * 100 + 50, cy * 100 + 50, currentOwner, 120);

                }
              }
            }

            setCurrentPosition({ x, y });
            setCurrentOwner(-1);
            setCurrentStartChess(null);
            checkWin();
          }
        }
      }
    }
    if (direction === 'up') {
      if (currentPosition.y > 0) putWire(currentPosition.x, currentPosition.y - 1);
    }
    if (direction === 'down') {
      if (currentPosition.y < 5) putWire(currentPosition.x, currentPosition.y + 1);
    }
    if (direction === 'left') {
      if (currentPosition.x > 0) putWire(currentPosition.x - 1, currentPosition.y);
    }
    if (direction === 'right') {
      if (currentPosition.x < 7) putWire(currentPosition.x + 1, currentPosition.y);
    }
  }
  const [currentOwner, setCurrentOwner] = React.useState(-1);

  const curPosPrimitives = React.useRef<number[]>([]);
  React.useEffect(() => {
    curPosPrimitives.current.forEach(primitive => {
      DeletePrimitive(primitive);
    })
    DrawSelectBox(currentPosition.x * 100 + 50, currentPosition.y * 100 + 50, currentOwner).then(r => {
      curPosPrimitives.current = r;
    })
  }, [currentPosition, currentOwner])
  const [currentStartChess, setCurrentStartChess] = React.useState<{ x: number, y: number } | null>(null);
  const handleSelect = () => {
    const currentChess = chessboard[currentPosition.y][currentPosition.x];
    if (currentChess.type === 'router') {
      const owner = currentChess.owner === undefined ? -1 : currentChess.owner;
      if (owner === currentOwner) {
        // 取消连线
        setCurrentOwner(-1);
      }
      else {
        // 开始连线
        if (allWiresPrimtives.current[owner]) {
          allWiresPrimtives.current[owner].forEach(primitive => {
            DeletePrimitive(primitive);
          })
          allWiresPrimtives.current[owner] = [];
        }
        setCurrentOwner(owner);
        setChessboard(chessboard.map(row => row.map(chess => chess.type === 'wire' && chess.owner === owner ? { type: 'empty' } : chess)));
        setCurrentStartChess({ x: currentPosition.x, y: currentPosition.y });
      }
    } else if (currentChess.type === 'wire') {
      if (currentOwner === -1) {
        // 删除已经连过的线
        const owner = currentChess.owner === undefined ? -1 : currentChess.owner;
        allWiresPrimtives.current[owner].forEach(primitive => {
          DeletePrimitive(primitive);
        })
        allWiresPrimtives.current[owner] = [];
        setChessboard(chessboard.map(row => row.map(chess => chess.type === 'wire' && chess.owner === owner ? { type: 'empty' } : chess)));
      } else {
        // 取消当前连线
        currentWiresPrimitive.current.forEach(primitive => {
          DeletePrimitive(primitive);
        })
        currentWiresPrimitive.current = [];
        setCurrentOwner(-1);
        setChessboard(chessboard.map(row => row.map(chess => chess.type === 'wire' && chess.owner === currentOwner ? { type: 'empty' } : chess)));
        setCurrentStartChess(null);
      }
    }
  }
  const gameEnd = React.useRef(false);
  const checkWin = async () => {
    const owners = new Set<number>();
    chessboard.forEach(row => row.forEach(chess => {
      if (chess.type === 'router') owners.add(chess.owner || -1);
    }));
    const connectedOwners = new Set<number>();
    chessboard.forEach(row => row.forEach(chess => {
      if (chess.type === 'wire') connectedOwners.add(chess.owner || -1);
    }));
    if (owners.size === connectedOwners.size) {
      gameEnd.current = true;
      await DrawWin();
      // alert('You Win!');
      location.reload()
    }
  }
  const Init = async () => {
    await ChangeColor('#ffffff');
    const newChessboard = GenerateChessboard();
    setChessboard(newChessboard)
    for (const row of newChessboard) {
      for (const chess of row) {
        if (chess.type === 'router') {
          await DrawRouter(row.indexOf(chess) * 100 + 50, newChessboard.indexOf(row) * 100 + 50, chess.owner === undefined ? -1 : chess.owner);
        }
      }
    }
    setCurrentPosition({ x: 0, y: 0 });
    document.getElementById('game')?.focus();
  }


  return <div
    id="game"
    onKeyDown={handleKeyPressed}
    tabIndex={0}
  >
    <div id="hover" style={{ opacity: start ? 0 : 1 }}>
      <div>{errorMessage}</div>
    </div>
    <div id="mousePosition">
      Engine: {engine_name};
      FPS: {fps}.</div>
    <p id='title'>
      <span style={{ color: "#2ecc71" }}>断</span>
      <span style={{ color: "#f1c40f" }}>线</span>
      <span style={{ color: "#74b9ff" }}>重</span>
      <span style={{ color: "#e74c3c" }}>连</span>
    </p>
    <div id='hide'>
      {
        chessboard.map((row, y) => row.map((chess, x) => {
          return <div key={`${x}-${y}`} className={`chess ${chess.type} ${chess.owner === undefined ? -2 : chess.owner}`} style={{ top: y * 100, left: x * 100 }} />
        }))
      }
      <div className={`chess current ${currentOwner}`} style={{ top: currentPosition.y * 100, left: currentPosition.x * 100 }} />
    </div>
    <div id="main">
      <div className='slider left'>
        <p>游戏背景</p>
        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;在遥远的未来，世界依赖一个庞大的智能能源系统来维持城市的运转。然而，这个系统的中心——被称为“能量之心”的能源枢纽——因一场未知的风暴而失去了平衡，导致许多节点断开连接。玩家将化身为一位被誉为“电之修复者”的天才电气修理工，手握特殊的能量导线工具，进入到错综复杂的电箱内。在这座小小的空间里，玩家不仅要面对狭窄的路径和有限的资源，还要小心翼翼地处理每一条线路，确保它们不相互干扰，重建能量流动，恢复整个系统的秩序。每一关的挑战不仅仅是对手艺的考验，更是一场智慧的较量，直面隐藏在电路中的神秘力量。
      </div>
      <canvas
        width={800}
        height={600}
        ref={canvasRef}
      />
      <div className='slider right'>
        <p>玩法介绍</p>
        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;在每一关的电路空间内，会有一个棋盘格，格子上散布着成对的电路节点，它们被赋予了不同的颜色，彼此呼应。你的任务是将同颜色的节点用横竖排列的线条连接，绘制出一条完整无缺的通路，既不能让线路碰撞，也不能相互交叠。你需要谨慎思考，用心操作，因为每一次成功的连接都意味着能量的复苏和城市的重生。
        <br /><br />
        <div id="key-help">
          <div className='key-item'>
            <div className={`key-icon ArrowUp`} />
            <div className="key-intro">向上移动</div>
          </div>
          <div className='key-item'>
            <div className={`key-icon ArrowDown`} />
            <div className="key-intro">向下移动</div>
          </div>
          <div className='key-item'>
            <div className={`key-icon ArrowLeft`} />
            <div className="key-intro">向左移动</div>
          </div>
          <div className='key-item'>
            <div className={`key-icon ArrowRight`} />
            <div className="key-intro">向右移动</div>
          </div>
          <div className='key-item'>
            <div className={`key-icon Space`} />
            <div className="key-intro">安装拆除</div>
          </div>
        </div>
        <br />
        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;你需要通过方向键来操作选择框，使用空格键来选中或取消选中节点，连接线路。你也可以通过选中导线来取消已经连接的节点。当所有的节点都被成功连接时，你就赢得了这一关的胜利。祝你好运！
      </div>
    </div>
    <div id='pressed-key-list'>
      {
        pressedKeys.map((key, index) => <div key={index} className={`key-icon ${key}`} />)
      }
    </div>
  </div>
}
